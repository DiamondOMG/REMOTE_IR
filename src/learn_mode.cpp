#include "learn_mode.h"
#include "led_module.h"
#include "profile_manager.h"
#include "storage_module.h"

// System buttons (not assignable)
static const int SYSTEM_BUTTONS[] = {4, 8, 12, 16};
static const int NUM_SYSTEM_BUTTONS = 4;

// Blink timing (ms)
static const unsigned long SLOW_BLINK_INTERVAL = 500;  // 1s cycle (500ms on, 500ms off)
static const unsigned long FAST_BLINK_INTERVAL = 100;   // 200ms cycle (rapid)

// State
static SystemMode current_mode = MODE_SEND;
static LearnedIRData learn_buffer = {};
static unsigned long last_blink_time = 0;
static bool blink_state = false;

void learn_mode_init() {
    current_mode = MODE_SEND;
    learn_buffer.valid = 0;
}

SystemMode get_current_mode() {
    return current_mode;
}

bool is_data_button(int btn) {
    for (int i = 0; i < NUM_SYSTEM_BUTTONS; i++) {
        if (btn == SYSTEM_BUTTONS[i]) return false;
    }
    return (btn >= 1 && btn <= 16);
}

void toggle_learn_mode() {
    if (current_mode == MODE_SEND) {
        // Enter Learn Mode
        current_mode = MODE_LEARN_WAITING;
        learn_buffer.valid = 0;
        last_blink_time = millis();
        blink_state = false;
        Serial.println(F(">>> LEARN MODE: ON (waiting for IR signal...) <<<"));
    } else {
        // Exit Learn Mode (from any learn state)
        current_mode = MODE_SEND;
        learn_buffer.valid = 0;
        
        // Restore profile LED display (solid, no blink)
        set_profile_led(get_current_profile_mask());
        Serial.println(F(">>> LEARN MODE: OFF <<<"));
    }
}

void learn_mode_update() {
    if (current_mode == MODE_SEND) return;

    unsigned long now = millis();
    unsigned long interval = (current_mode == MODE_LEARN_WAITING) 
                              ? SLOW_BLINK_INTERVAL 
                              : FAST_BLINK_INTERVAL;

    if (now - last_blink_time >= interval) {
        last_blink_time = now;
        blink_state = !blink_state;

        if (blink_state) {
            set_profile_led(get_current_profile_mask());
        } else {
            set_profile_led(0); // All LEDs off
        }
    }
}

const LearnedIRData* get_learn_buffer() {
    return &learn_buffer;
}

void learn_store_signal(const LearnedIRData &data) {
    learn_buffer = data;
    learn_buffer.valid = 1;
    current_mode = MODE_LEARN_RECEIVED;
    last_blink_time = millis();
    blink_state = false;
    
    Serial.println(F(">>> IR CAPTURED! Press a button (1-12) to assign <<<"));
    ir_print_data(learn_buffer);
}

void learn_assign_button(int btn) {
    if (!learn_buffer.valid) return;

    int8_t slot = button_to_slot(btn);
    if (slot < 0) return;

    // Save to storage (RAM)
    storage_save(slot, learn_buffer);
    
    // Write all slots of current profile to Flash
    storage_save_profile(get_current_profile());

    Serial.print(F(">>> ASSIGNED to Profile "));
    Serial.print(get_current_profile());
    Serial.print(F(", Button "));
    Serial.print(btn);
    Serial.print(F(" (Slot "));
    Serial.print(slot);
    Serial.println(F(") <<<"));
    ir_print_data(learn_buffer);

    // Clear buffer and go back to waiting
    learn_buffer.valid = 0;
    current_mode = MODE_LEARN_WAITING;
    last_blink_time = millis();
    blink_state = false;
    
    Serial.println(F("Buffer cleared. Waiting for next IR signal..."));
}

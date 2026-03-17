#include <Arduino.h>
#include "led_module.h"
#include "button_module.h"
#include "ir_module.h"
#include "profile_manager.h"
#include "learn_mode.h"
#include "storage_module.h"

LearnedIRData temp_ir_data;

void setup() {
    delay(6000);
    Serial.begin(115200);
    Serial.println("System starting...");

    // Initialize Modules
    led_init();
    test_order_led(); // Startup LED sequence
    button_init();
    init_ir();
    storage_init();   // Load Flash data FIRST
    learn_mode_init();
    profile_init();   // Then load Profile 1 from Flash cache

    Serial.println("All Modules Initialized.");
    Serial.println("Mode: SEND | S4=Learn | S8=Down | S12=Up | S16=TestSend");
}

void loop() {
    // Non-blocking LED blink update (for learn mode)
    learn_mode_update();

    SystemMode mode = get_current_mode();

    // 1. Check IR Receiver
    if (ir_poll_received(&temp_ir_data)) {
        if (mode == MODE_LEARN_WAITING) {
            // In learn mode waiting: store signal to buffer
            learn_store_signal(temp_ir_data);
        } else if (mode == MODE_SEND) {
            // In send mode: just log it
            Serial.println();
            Serial.println(">>> IR SIGNAL RECEIVED <<<");
            ir_print_data(temp_ir_data);
        }
        // In MODE_LEARN_RECEIVED: ignore new signals until button is pressed
    }

    // 2. Check Button Matrix
    int btn = get_button_pressed();
    if (btn != 0) {
        Serial.print("Button Pressed: ");
        Serial.println(btn);

        // System button handling (works in all modes)
        switch(btn) {
            case 4: // Toggle Learn Mode
                toggle_learn_mode();
                return;

            case 8: // Profile DOWN (แดง -> เหลือง -> เขียว ...)
                if (mode == MODE_SEND) profile_down();
                return;
            
            case 12: // Profile UP (เขียว -> เหลือง -> แดง ...)
                if (mode == MODE_SEND) profile_up();
                return;

            case 16: // Reserved (future use)
                return;
        }

        // Data button handling (1-3, 5-7, 9-11, 13-15)
        if (is_data_button(btn)) {
            if (mode == MODE_LEARN_RECEIVED) {
                // Assign the learned signal to this button
                learn_assign_button(btn);
            } else if (mode == MODE_SEND) {
                // Send stored IR signal from storage
                int8_t slot = button_to_slot(btn);
                if (slot >= 0) {
                    LearnedIRData send_data;
                    if (storage_load(slot, &send_data)) {
                        Serial.print("Sending slot ");
                        Serial.print(slot);
                        Serial.println("...");
                        ir_send_frame(send_data);
                    } else {
                        Serial.print("Slot ");
                        Serial.print(slot);
                        Serial.println(" is empty");
                    }
                }
            }
        }
    }
}

#include <Arduino.h>
#include "led_module.h"
#include "button_module.h"
#include "ir_module.h"
#include "profile_manager.h"

LearnedIRData temp_ir_data;

void setup() {
    delay(6000);
    Serial.begin(115200);
    Serial.println("System starting...");

    // Initialize Modules
    led_init();
    test_order_led(); // Startup LED sequence
    profile_init();   // Start at Profile 0
    button_init();
    init_ir();
    
    // Pre-load NEC signal for testing IR Send (Button 16)
    temp_ir_data.valid = 1;
    temp_ir_data.protocol = 8; // NEC protocol index from IRProtocol.h
    temp_ir_data.address = 0x56;
    temp_ir_data.command = 0x0;
    temp_ir_data.numberOfBits = 32;
    temp_ir_data.flags = 0x0;

    Serial.println("All Modules Initialized.");
}

void loop() {
    // 1. Check IR Receiver
    if (ir_poll_received(&temp_ir_data)) {
        Serial.println();
        Serial.println(">>> IR SIGNAL RECEIVED <<<");
        ir_print_data(temp_ir_data);
    }

    // 2. Check Button Matrix
    int btn = get_button_pressed();
    if (btn != 0) {
        Serial.print("Button Pressed: ");
        Serial.println(btn);
        
        switch(btn) {
            case 8: // Profile DOWN (แดง -> เหลือง -> เขียว ...)
                profile_down();
                break;
            
            case 12: // Profile UP (เขียว -> เหลือง -> แดง ...)
                profile_up();
                break;

            case 16: // Test Send
                if (temp_ir_data.valid) {
                    Serial.println("Testing Send (Button 16)...");
                    ir_send_frame(temp_ir_data);
                } else {
                    Serial.println("No valid IR data to send!");
                }
                break;

            default:
                // Visual feedback removed for other buttons
                break;
        }
    }
}

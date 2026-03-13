#include <Arduino.h>
#include "led_module.h"
#include "button_module.h"
#include "ir_module.h"

LearnedIRData temp_ir_data;

void setup() {
    delay(6000);
    Serial.begin(115200);
    Serial.println("System starting...");

    // Initialize Modules
    led_init();
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
    Serial.println("Ready to test IR Send: Press Button 16 to fire NEC 0x56:0x0");
}

void loop() {
    // 1. Check IR Receiver
    if (ir_poll_received(&temp_ir_data)) {
        Serial.println();
        Serial.println(">>> IR SIGNAL RECEIVED <<<");
        ir_print_data(temp_ir_data);
        
        // Blink Green LED to indicate reception
        digitalWrite(PA4, HIGH);
        delay(100);
        digitalWrite(PA4, LOW);
    }

    // 2. Check Button Matrix
    int btn = get_button_pressed();
    if (btn != 0) {
        Serial.print("Button Pressed: ");
        Serial.println(btn);
        
        // If button 16 is pressed, try to send the last learned signal
        if (btn == 16) {
            if (temp_ir_data.valid) {
                Serial.println("Testing Send (Button 16)...");
                ir_send_frame(temp_ir_data);
                
                // Blink Blue LED for Send
                digitalWrite(PA6, HIGH);
                delay(100);
                digitalWrite(PA6, LOW);
            } else {
                Serial.println("No valid IR data to send!");
            }
        } else {
            // Visual feedback for other buttons
            digitalWrite(PA0, HIGH); 
            delay(50);
            digitalWrite(PA0, LOW);
        }
    }
}

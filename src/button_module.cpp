#include "button_module.h"

static const uint8_t ROW_PINS[] = {ROW1, ROW2, ROW3, ROW4};
static const uint8_t COL_PINS[] = {COL1, COL2, COL3, COL4};

void button_init() {
    // STM32 specific: Disable JTAG to use PA15, PB3, PB4 as normal GPIO
    // This is required for Bluepill to use these specific pins
    #ifdef HAL_AFIO_MODULE_ENABLED
        __HAL_RCC_AFIO_CLK_ENABLE();
        __HAL_AFIO_REMAP_SWJ_NOJTAG();
    #endif

    for (uint8_t i = 0; i < 4; i++) {
        pinMode(ROW_PINS[i], INPUT_PULLUP);
    }
    for (uint8_t i = 0; i < 4; i++) {
        pinMode(COL_PINS[i], OUTPUT);
        digitalWrite(COL_PINS[i], HIGH);
    }
}

int get_button_pressed() {
    for (uint8_t c = 0; c < 4; c++) {
        digitalWrite(COL_PINS[c], LOW);
        for (uint8_t r = 0; r < 4; r++) {
            // Fix: Changed digitalWrite to digitalRead
            if (digitalRead(ROW_PINS[r]) == LOW) {
                delay(50); // Debounce
                while(digitalRead(ROW_PINS[r]) == LOW); // Wait for release
                digitalWrite(COL_PINS[c], HIGH);
                
                // Remapped to match physical buttons (Inverting Rows)
                return ((3 - r) * 4) + (c + 1); 
            }
        }
        digitalWrite(COL_PINS[c], HIGH);
    }
    return 0;
}

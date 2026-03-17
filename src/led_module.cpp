#include "led_module.h"

static const uint8_t LED_PINS[] = {
    LED_RED_PIN,
    LED_YELLOW_PIN,
    LED_GREEN_PIN,
    LED_BLUE_PIN,
    LED_WHITE_PIN
};

static const uint8_t NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

void led_init() {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        pinMode(LED_PINS[i], OUTPUT);
        digitalWrite(LED_PINS[i], LOW); // Start with all OFF
    }
}

void test_order_led() {
    // Forward sequence
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], HIGH);
        delay(250);
        digitalWrite(LED_PINS[i], LOW);
    }
    
    // Backward sequence
    for (int8_t i = NUM_LEDS - 2; i >= 1; i--) {
        digitalWrite(LED_PINS[i], HIGH);
        delay(250);
        digitalWrite(LED_PINS[i], LOW);
    }
}

/**
 * @brief Sets LEDs based on profile index (5-bit binary)
 * @param profile 0-31
 */
void set_profile_led(uint8_t profile) {
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
        bool state = (profile >> i) & 0x01;
        digitalWrite(LED_PINS[i], state ? HIGH : LOW);
    }
}

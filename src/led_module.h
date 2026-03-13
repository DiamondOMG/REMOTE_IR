#pragma once
#include <Arduino.h>

// LED Pin Definitions
#define LED_RED_PIN    PA0
#define LED_YELLOW_PIN PA2
#define LED_GREEN_PIN  PA4
#define LED_BLUE_PIN   PA6
#define LED_WHITE_PIN  PB0

// Function Prototypes
void led_init();
void test_order_led();
void set_profile_led(uint8_t profile);

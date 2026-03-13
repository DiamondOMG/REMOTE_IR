#pragma once

#include <Arduino.h>
#include "remote_storage.h"

// LED Config - Pin Definitions
#define LED_RED PA0
#define LED_YELLOW PA2
#define LED_GREEN PA4
#define LED_BLUE PA6
#define LED_WHITE PB0

void initLeds();
void testLedSequence(int delayMs);

// Profile index 0..30 -> bitmask 1..31 (bit0=R, bit1=Y, bit2=G, bit3=B, bit4=W)
void led_update_status(uint8_t profileIndex, bool learningMode, bool pendingLearned);


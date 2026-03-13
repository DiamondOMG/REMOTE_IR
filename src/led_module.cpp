#include "led_module.h"

const int ALL_LED_PINS[] = {LED_RED, LED_YELLOW, LED_GREEN, LED_BLUE, LED_WHITE};
const int NUM_LEDS = 5;

void initLeds() {
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(ALL_LED_PINS[i], OUTPUT);
        digitalWrite(ALL_LED_PINS[i], LOW);
    }
}

// ตั้งค่า LED ตาม bitmask (bit0=RED, bit1=YELLOW, bit2=GREEN, bit3=BLUE, bit4=WHITE)
static void setLedPattern(uint8_t pattern, bool on) {
    for (int i = 0; i < NUM_LEDS; i++) {
        bool shouldLight = (pattern >> i) & 1;
        digitalWrite(ALL_LED_PINS[i], (shouldLight && on) ? HIGH : LOW);
    }
}

// Lookup table: เรียงจาก 1 ดวง -> 2 ดวง -> 3 ดวง -> 4 ดวง -> 5 ดวง
// bit0=R, bit1=Y, bit2=G, bit3=B, bit4=W
static const uint8_t PROFILE_PATTERN[31] = {
    // 1 ดวง (5)
    0x01, 0x02, 0x04, 0x08, 0x10,
    // 2 ดวง (10)
    0x03, 0x05, 0x09, 0x11, 0x06,
    0x0A, 0x12, 0x0C, 0x14, 0x18,
    // 3 ดวง (10)
    0x07, 0x0B, 0x13, 0x0D, 0x15,
    0x19, 0x0E, 0x16, 0x1A, 0x1C,
    // 4 ดวง (5)
    0x0F, 0x17, 0x1B, 0x1D, 0x1E,
    // 5 ดวง (1)
    0x1F
};

void led_update_status(uint8_t profileIndex, bool learningMode, bool pendingLearned) {
    if (profileIndex >= REMOTE_PROFILE_COUNT) {
        for (int i = 0; i < NUM_LEDS; i++) {
            digitalWrite(ALL_LED_PINS[i], LOW);
        }
        return;
    }

    uint8_t pattern = PROFILE_PATTERN[profileIndex];

    if (!learningMode) {
        setLedPattern(pattern, true);
        return;
    }

    // Learning mode: กระพริบ
    uint32_t now = millis();
    bool ledOn;
    if (pendingLearned) {
        ledOn = ((now / 80) % 2) == 0;   // กระพริบเร็ว = จับ IR ได้แล้ว
    } else {
        ledOn = ((now / 500) % 2) == 0;  // กระพริบช้า = รอ IR
    }
    setLedPattern(pattern, ledOn);
}

void testLedSequence(int delayMs) {
    for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(ALL_LED_PINS[i], HIGH);
        delay(delayMs);
        digitalWrite(ALL_LED_PINS[i], LOW);
    }

    for (int i = NUM_LEDS - 2; i >= 0; i--) {
        digitalWrite(ALL_LED_PINS[i], HIGH);
        delay(delayMs);
        digitalWrite(ALL_LED_PINS[i], LOW);
    }
}

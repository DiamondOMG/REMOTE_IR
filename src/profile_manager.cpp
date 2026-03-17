#include "profile_manager.h"
#include "led_module.h"
#include "storage_module.h"

// Mapping profiles to bitmasks (1-31) based on user's intuitive order:
// Singles, then Pairs, then Triples, then Quads, then All.
static const uint8_t PROFILE_MAP[31] = {
    1, 2, 4, 8, 16,               // 1 LED: R, Y, G, B, W
    3, 5, 9, 17, 6, 10, 18, 12, 20, 24, // 2 LEDs: RY, RG, RB, RW, YG, YB, YW, GB, GW, BW
    7, 11, 19, 13, 21, 25, 14, 22, 26, 28, // 3 LEDs ...
    15, 23, 27, 29, 30,           // 4 LEDs ...
    31                            // 5 LEDs: All
};

static int8_t current_map_index = 0; // 0 to 30 (mapping to 31 profiles)
static const uint8_t TOTAL_PROFILES = 31;

void profile_init() {
    current_map_index = 0; // Start at first profile (Red)
    set_profile_led(PROFILE_MAP[current_map_index]);
    storage_load_profile(current_map_index + 1); // Load from Flash
    Serial.print(F("Profile Initialized: "));
    Serial.print(current_map_index + 1);
    Serial.print(F(" (Mask: "));
    Serial.print(PROFILE_MAP[current_map_index]);
    Serial.println(F(")"));
}

void profile_up() {
    current_map_index++;
    if (current_map_index >= TOTAL_PROFILES) {
        current_map_index = 0; // Wrap to first
    }
    set_profile_led(PROFILE_MAP[current_map_index]);
    storage_load_profile(current_map_index + 1); // Load new profile from Flash
    Serial.print(F("Profile UP -> "));
    Serial.println(current_map_index + 1);
}

void profile_down() {
    current_map_index--;
    if (current_map_index < 0) {
        current_map_index = TOTAL_PROFILES - 1; // Wrap to last
    }
    set_profile_led(PROFILE_MAP[current_map_index]);
    storage_load_profile(current_map_index + 1); // Load new profile from Flash
    Serial.print(F("Profile DOWN -> "));
    Serial.println(current_map_index + 1);
}

uint8_t get_current_profile() {
    return current_map_index + 1; // Return 1-31 for user/storage
}

uint8_t get_current_profile_mask() {
    return PROFILE_MAP[current_map_index];
}

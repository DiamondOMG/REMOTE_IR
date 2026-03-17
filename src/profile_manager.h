#pragma once
#include <Arduino.h>

/**
 * @brief Initialize the profile manager
 */
void profile_init();

/**
 * @brief Increment current profile (0-31) and update LEDs
 */
void profile_up();

/**
 * @brief Decrement current profile (0-31) and update LEDs
 */
void profile_down();

/**
 * @brief Get the currently active profile index
 * @return uint8_t 1-31
 */
uint8_t get_current_profile();

/**
 * @brief Get the LED bitmask for the current profile
 * @return uint8_t bitmask (1-31) to pass to set_profile_led()
 */
uint8_t get_current_profile_mask();

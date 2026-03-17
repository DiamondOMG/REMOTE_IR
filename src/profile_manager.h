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
 * @return uint8_t 0-31
 */
uint8_t get_current_profile();

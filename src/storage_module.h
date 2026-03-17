#pragma once
#include <Arduino.h>
#include "ir_module.h"

// 12 usable buttons per profile (excluding system buttons 4, 8, 12, 16)
#define SLOTS_PER_PROFILE 12

/**
 * @brief Initialize storage module
 */
void storage_init();

/**
 * @brief Convert button number (1-15, skip system) to slot index (0-11)
 * @param btn Button number
 * @return int8_t Slot index (0-11), or -1 if invalid/system button
 */
int8_t button_to_slot(int btn);

/**
 * @brief Save IR data to a slot for the current profile
 * @param slot_index 0-11
 * @param data IR data to store
 */
void storage_save(uint8_t slot_index, const LearnedIRData &data);

/**
 * @brief Load IR data from a slot for the current profile
 * @param slot_index 0-11
 * @param outData Pointer to receive the data
 * @return true if slot has valid data
 */
bool storage_load(uint8_t slot_index, LearnedIRData *outData);

/**
 * @brief Clear all slots for the current profile
 */
void storage_clear_profile();

/**
 * @brief Load profile data from Flash to RAM cache
 * @param profile_id 1-31
 */
void storage_load_profile(uint8_t profile_id);

/**
 * @brief Save RAM cache data to Flash for the current profile
 * @param profile_id 1-31
 */
void storage_save_profile(uint8_t profile_id);

/**
 * @brief Save the last active profile index to Flash
 * @param map_index 0-30
 */
void storage_save_last_profile(uint8_t map_index);

/**
 * @brief Load the last active profile index from Flash
 * @return int8_t 0-30, or 0 if not set
 */
int8_t storage_load_last_profile();

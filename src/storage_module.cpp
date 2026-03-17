#include <EEPROM.h>
#include "storage_module.h"
#include "ir_module.h"

// RAM storage for current profile (12 slots)
static LearnedIRData slots[SLOTS_PER_PROFILE];

// Helper to calculate EEPROM address for a profile
// Each profile takes 96 bytes (12 * 8)
static uint16_t get_profile_addr(uint8_t profile_id) {
    if (profile_id < 1) profile_id = 1;
    return (profile_id - 1) * 96; 
}

void storage_init() {
    // Load existing Flash data into EEPROM buffer
    eeprom_buffer_fill();
    Serial.println(F("Flash Storage Initialized."));
}

void storage_load_profile(uint8_t profile_id) {
    uint16_t addr = get_profile_addr(profile_id);
    
    for (uint8_t i = 0; i < SLOTS_PER_PROFILE; i++) {
        EEPROM.get(addr + (i * 8), slots[i]);
        // If data is invalid (e.g. fresh flash), ensure valid flag is 0
        if (slots[i].valid == 0xFF) slots[i].valid = 0;
    }
    
    Serial.print(F("Loaded Profile "));
    Serial.print(profile_id);
    Serial.print(F(" from Flash address "));
    Serial.println(addr);
}

void storage_save_profile(uint8_t profile_id) {
    uint16_t addr = get_profile_addr(profile_id);
    
    for (uint8_t i = 0; i < SLOTS_PER_PROFILE; i++) {
        EEPROM.put(addr + (i * 8), slots[i]);
    }
    
    // Commit RAM buffer to Flash
    eeprom_buffer_flush();

    Serial.print(F("Saved Profile "));
    Serial.print(profile_id);
    Serial.print(F(" to Flash address "));
    Serial.println(addr);
}

int8_t button_to_slot(int btn) {
    if (btn < 1 || btn > 15) return -1;
    
    // System buttons
    if (btn == 4 || btn == 8 || btn == 12) return -1;

    // btn:  1  2  3 | 5  6  7 | 9 10 11 | 13 14 15
    // slot: 0  1  2 | 3  4  5 | 6  7  8 |  9 10 11
    int row = (btn - 1) / 4;
    int col = (btn - 1) % 4;
    if (col == 3) return -1;
    
    return (row * 3) + col;
}
void storage_save(uint8_t slot_index, const LearnedIRData &data) {
    if (slot_index >= SLOTS_PER_PROFILE) return;
    
    slots[slot_index] = data;
    slots[slot_index].valid = 1;
    
    Serial.print(F("Stored to slot "));
    Serial.println(slot_index);
}

bool storage_load(uint8_t slot_index, LearnedIRData *outData) {
    if (slot_index >= SLOTS_PER_PROFILE || outData == nullptr) return false;
    
    if (!slots[slot_index].valid) return false;
    
    *outData = slots[slot_index];
    return true;
}

void storage_clear_profile() {
    for (uint8_t i = 0; i < SLOTS_PER_PROFILE; i++) {
        slots[i].valid = 0;
    }
    Serial.println(F("Profile storage cleared"));
}

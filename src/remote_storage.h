#pragma once

#include <Arduino.h>

#include "ir_module.h"

static const uint8_t REMOTE_PROFILE_COUNT = 31;
static const uint8_t REMOTE_BUTTON_COUNT = 16;

bool storage_init();

bool storage_read_slot(uint8_t profileIndex, uint8_t buttonIndex, LearnedIRData *outData);

bool storage_write_slot(uint8_t profileIndex, uint8_t buttonIndex, const LearnedIRData &data);

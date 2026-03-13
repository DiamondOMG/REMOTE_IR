#pragma once

#include <Arduino.h>

// Use TIM1 for IR send timing on STM32F103.
#define STM32_TIMER_NUMBER 1

#define IR_RECEIVE_PIN PB11
#define IR_SEND_PIN PA8

struct LearnedIRData {
    uint8_t valid;
    uint8_t protocol;
    uint16_t address;
    uint16_t command;
    uint16_t numberOfBits;
    uint8_t flags;
};

void init_ir();

// Poll receiver and return first non-repeat decoded frame.
bool ir_poll_received(LearnedIRData *outData);
// Returns true if any IR frame is currently available (and discards it).
bool ir_has_activity();

// Send full frame from stored data.
bool ir_send_frame(const LearnedIRData &data);

// Send protocol-specific repeat when supported; fallback to full frame.
bool ir_send_repeat(const LearnedIRData &data);

void ir_print_data(const LearnedIRData &data);

#pragma once
#include <Arduino.h>
#include "ir_module.h"

// System operating modes
enum SystemMode {
    MODE_SEND,           // Normal: press 1-12 buttons to send IR
    MODE_LEARN_WAITING,  // Learn: waiting for IR signal (slow blink)
    MODE_LEARN_RECEIVED  // Learn: IR received, waiting for button assign (fast blink)
};

/**
 * @brief Initialize learn mode system
 */
void learn_mode_init();

/**
 * @brief Get the current system mode
 */
SystemMode get_current_mode();

/**
 * @brief Toggle learn mode on/off (called when S4 is pressed)
 */
void toggle_learn_mode();

/**
 * @brief Must be called every loop() for non-blocking LED blink
 */
void learn_mode_update();

/**
 * @brief Check if a button is a "data" button (not system button)
 * @param btn Button number (1-16)
 * @return true if btn is one of the 12 usable buttons
 */
bool is_data_button(int btn);

/**
 * @brief Get the learned IR data buffer (read-only)
 */
const LearnedIRData* get_learn_buffer();

/**
 * @brief Store IR data into learn buffer, switch to RECEIVED state
 */
void learn_store_signal(const LearnedIRData &data);

/**
 * @brief Assign learned signal to a button, clear buffer, back to WAITING
 * @param btn The data button (1-15, excluding system buttons)
 */
void learn_assign_button(int btn);

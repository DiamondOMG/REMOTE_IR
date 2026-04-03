#pragma once

#include <Arduino.h>
#include "ir_module.h"

/**
 * @brief Initialize Sweep Mode variables
 */
void sweep_mode_init();

/**
 * @brief Enter Sweep Mode
 */
void sweep_mode_enter();

/**
 * @brief Handles button presses during Sweep Mode
 * @param btn The button pressed (1-16)
 */
void sweep_mode_handle_button(int btn);

/**
 * @brief Returns the current generated IR signal that was just swept
 */
LearnedIRData sweep_mode_get_current_signal();

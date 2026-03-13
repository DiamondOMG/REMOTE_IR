#pragma once

#include <Arduino.h>

// Button Config - Pin Definitions
// Rows (PA15 requires JTAG disable via __HAL_AFIO_REMAP_SWJ_NOJTAG in setup)
#define BTN_ROW_0 PA15
#define BTN_ROW_1 PB3
#define BTN_ROW_2 PB4
#define BTN_ROW_3 PB5

// Columns
#define BTN_COL_0 PB6
#define BTN_COL_1 PB7
#define BTN_COL_2 PB8
#define BTN_COL_3 PB9

// Extra Buttons
#define BTN_EXTRA_1 PB12 // Mode toggle
#define BTN_EXTRA_2 PB13 // Profile change (down)
#define BTN_EXTRA_3 PB14 // Profile change (up)

void initButtons();
void check_buttons();

// Returns one-shot stable press event (0 = none).
int take_button_pressed_event();

// Returns one-shot stable release event (0 = none).
int take_button_released_event();

// Returns currently pressed stable button (0 = none).
int get_stable_pressed_button();

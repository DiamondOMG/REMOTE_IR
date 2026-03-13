#pragma once
#include <Arduino.h>

/*
 * Button Matrix 4x4
 * Pins: PA15, PB3, PB4, PB5, PB6, PB7, PB8, PB9
 * 
 * Rows: PA15, PB3, PB4, PB5 (Input Pullup)
 * Cols: PB6, PB7, PB8, PB9 (Output)
 */

#define ROW1 PA15
#define ROW2 PB3
#define ROW3 PB4
#define ROW4 PB5

#define COL1 PB6
#define COL2 PB7
#define COL3 PB8
#define COL4 PB9

// Function Prototypes
void button_init();
int get_button_pressed(); // Returns 1-16 or 0 if none

#include "button_module.h"

// Rows
const int ROW_PINS[4] = {BTN_ROW_0, BTN_ROW_1, BTN_ROW_2, BTN_ROW_3};
// Columns
const int COL_PINS[4] = {BTN_COL_0, BTN_COL_1, BTN_COL_2, BTN_COL_3};

static const uint32_t PRESS_DEBOUNCE_MS = 30;
static const uint32_t RELEASE_DEBOUNCE_MS = 20;

static int s_candidate_button = 0;
static int s_stable_button = 0;
static int s_pressed_event = 0;
static int s_released_event = 0;
static uint32_t s_candidate_since_ms = 0;

static int scanKeypadRaw() {
    // Extra buttons are active-low and returned as 17/18/19.
    if (digitalRead(BTN_EXTRA_1) == LOW) return 17;
    if (digitalRead(BTN_EXTRA_2) == LOW) return 18;
    if (digitalRead(BTN_EXTRA_3) == LOW) return 19;

    for (int r = 0; r < 4; r++) {
        digitalWrite(ROW_PINS[r], LOW);
        delayMicroseconds(5);

        for (int c = 0; c < 4; c++) {
            if (digitalRead(COL_PINS[c]) == LOW) {
                digitalWrite(ROW_PINS[r], HIGH);
                return ((3 - r) * 4) + (c + 1);
            }
        }

        digitalWrite(ROW_PINS[r], HIGH);
    }

    return 0;
}

void initButtons() {
    for (int i = 0; i < 4; i++) {
        pinMode(ROW_PINS[i], OUTPUT);
        digitalWrite(ROW_PINS[i], HIGH);
    }

    for (int i = 0; i < 4; i++) {
        pinMode(COL_PINS[i], INPUT_PULLUP);
    }

    pinMode(BTN_EXTRA_1, INPUT_PULLUP);
    pinMode(BTN_EXTRA_2, INPUT_PULLUP);
    pinMode(BTN_EXTRA_3, INPUT_PULLUP);

    s_candidate_button = 0;
    s_stable_button = 0;
    s_pressed_event = 0;
    s_released_event = 0;
    s_candidate_since_ms = millis();
}

void check_buttons() {
    int current_button = scanKeypadRaw();
    uint32_t now_ms = millis();

    if (current_button != s_candidate_button) {
        s_candidate_button = current_button;
        s_candidate_since_ms = now_ms;
        return;
    }

    uint32_t elapsed = now_ms - s_candidate_since_ms;
    uint32_t required_ms = (s_candidate_button == 0) ? RELEASE_DEBOUNCE_MS : PRESS_DEBOUNCE_MS;
    if (elapsed < required_ms) {
        return;
    }

    if (s_candidate_button != s_stable_button) {
        int old_button = s_stable_button;
        s_stable_button = s_candidate_button;

        if (s_stable_button != 0) {
            s_pressed_event = s_stable_button;
            Serial.print(F("Button Pressed: "));
            Serial.println(s_stable_button);
        }

        if (old_button != 0 && s_stable_button == 0) {
            s_released_event = old_button;
        }
    }
}

int take_button_pressed_event() {
    int ev = s_pressed_event;
    s_pressed_event = 0;
    return ev;
}

int take_button_released_event() {
    int ev = s_released_event;
    s_released_event = 0;
    return ev;
}

int get_stable_pressed_button() {
    return s_stable_button;
}

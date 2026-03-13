#include <Arduino.h>

#include "button_module.h"
#include "ir_module.h"
#include "led_module.h"
#include "remote_storage.h"

namespace {

enum AppMode : uint8_t {
  MODE_SEND = 0,
  MODE_LEARNING = 1,
};

static const int BUTTON_MODE_TOGGLE = 17;
static const int BUTTON_PROFILE_DOWN = 18;
static const int BUTTON_PROFILE_UP = 19;

static const uint32_t HOLD_REPEAT_START_MS = 350;
static const uint32_t HOLD_REPEAT_INTERVAL_MS = 110;

static const uint32_t LEARNING_IDLE_TIMEOUT_MS = 60000;
static const uint32_t LEARNING_ASSIGN_TIMEOUT_MS = 15000;
static const uint32_t LEARNING_QUIET_BEFORE_ARM_MS = 300;

AppMode g_mode = MODE_SEND;
uint8_t g_activeProfile = 0;

LearnedIRData g_pendingLearnedData = {};
bool g_hasPendingLearnedData = false;

uint32_t g_learningModeSinceMs = 0;
uint32_t g_pendingLearnedSinceMs = 0;
uint32_t g_ignoreIrUntilMs = 0;
bool g_waitForIrQuiet = false;
uint32_t g_lastIrActivityMs = 0;

int g_holdButton = 0;
uint32_t g_nextRepeatAtMs = 0;


const __FlashStringHelper *modeName(AppMode mode) {
  return (mode == MODE_LEARNING) ? F("LEARNING") : F("SEND");
}

void clearHoldState() {
  g_holdButton = 0;
  g_nextRepeatAtMs = 0;
}

void enterSendMode() {
  g_mode = MODE_SEND;
  g_hasPendingLearnedData = false;
  memset(&g_pendingLearnedData, 0, sizeof(g_pendingLearnedData));
  g_waitForIrQuiet = false;
  clearHoldState();
  Serial.println(F("Mode -> SEND"));
}

void enterLearningMode() {
  g_mode = MODE_LEARNING;
  g_learningModeSinceMs = millis();
  g_hasPendingLearnedData = false;
  memset(&g_pendingLearnedData, 0, sizeof(g_pendingLearnedData));
  g_waitForIrQuiet = false;
  clearHoldState();
  Serial.println(F("Mode -> LEARNING"));
}

void toggleMode() {
  if (g_mode == MODE_SEND) {
    enterLearningMode();
  } else {
    enterSendMode();
  }
}

void printProfile() {
  Serial.print(F("Active profile: "));
  Serial.println(g_activeProfile + 1);
}

void nextProfileDown() {
  g_activeProfile = (g_activeProfile + 1) % REMOTE_PROFILE_COUNT;
  g_hasPendingLearnedData = false;
  memset(&g_pendingLearnedData, 0, sizeof(g_pendingLearnedData));
  g_waitForIrQuiet = false;
  clearHoldState();
  printProfile();
}

void nextProfileUp() {
  g_activeProfile = (g_activeProfile + REMOTE_PROFILE_COUNT - 1) % REMOTE_PROFILE_COUNT;
  g_hasPendingLearnedData = false;
  memset(&g_pendingLearnedData, 0, sizeof(g_pendingLearnedData));
  g_waitForIrQuiet = false;
  clearHoldState();
  printProfile();
}

bool sendFromSlot(uint8_t button, bool repeatFrame) {
  LearnedIRData slot = {};
  if (!storage_read_slot(g_activeProfile, button, &slot) || !slot.valid) {
    if (!repeatFrame) {
      Serial.print(F("No learned IR on profile "));
      Serial.print(g_activeProfile + 1);
      Serial.print(F(", button "));
      Serial.println(button);
    }
    return false;
  }

  bool ok = repeatFrame ? ir_send_repeat(slot) : ir_send_frame(slot);
  if (!ok) {
    Serial.println(F("IR send failed"));
  }
  return ok;
}

void onKeypadPressSendMode(int button) {
  if (button < 1 || button > REMOTE_BUTTON_COUNT) {
    return;
  }

  bool sent = sendFromSlot(static_cast<uint8_t>(button), false);
  if (sent) {
    g_holdButton = button;
    g_nextRepeatAtMs = millis() + HOLD_REPEAT_START_MS;
  } else {
    clearHoldState();
  }
}

void onKeypadPressLearningMode(int button) {
  if (button < 1 || button > REMOTE_BUTTON_COUNT) {
    return;
  }

  if (!g_hasPendingLearnedData) {
    Serial.println(F("Learning: waiting for IR frame first"));
    return;
  }

  if (storage_write_slot(g_activeProfile, static_cast<uint8_t>(button), g_pendingLearnedData)) {
    Serial.print(F("Saved profile "));
    Serial.print(g_activeProfile + 1);
    Serial.print(F(" button "));
    Serial.println(button);
  } else {
    Serial.println(F("Flash save failed"));
  }

  g_hasPendingLearnedData = false;
  memset(&g_pendingLearnedData, 0, sizeof(g_pendingLearnedData));
  g_learningModeSinceMs = millis();
  g_ignoreIrUntilMs = g_learningModeSinceMs + 400;
  g_waitForIrQuiet = true;
  g_lastIrActivityMs = g_learningModeSinceMs;
}

void processPressEvent(int button) {
  if (button == BUTTON_MODE_TOGGLE) {
    toggleMode();
    return;
  }

  if (button == BUTTON_PROFILE_DOWN) {
    nextProfileDown();
    return;
  }

  if (button == BUTTON_PROFILE_UP) {
    nextProfileUp();
    return;
  }

  if (button >= 1 && button <= REMOTE_BUTTON_COUNT) {
    if (g_mode == MODE_SEND) {
      onKeypadPressSendMode(button);
    } else {
      onKeypadPressLearningMode(button);
    }
  }
}

void processReleaseEvent(int button) {
  if (button == g_holdButton) {
    clearHoldState();
  }
}

void updateHoldRepeat() {
  if (g_mode != MODE_SEND || g_holdButton == 0) {
    return;
  }

  int held = get_stable_pressed_button();
  if (held != g_holdButton) {
    clearHoldState();
    return;
  }

  uint32_t now = millis();
  if (now < g_nextRepeatAtMs) {
    return;
  }

  if (sendFromSlot(static_cast<uint8_t>(g_holdButton), true)) {
    g_nextRepeatAtMs = now + HOLD_REPEAT_INTERVAL_MS;
  } else {
    clearHoldState();
  }
}

void updateLearningReceiver() {
  if (g_mode != MODE_LEARNING || g_hasPendingLearnedData) {
    return;
  }

  uint32_t now = millis();
  if (now < g_ignoreIrUntilMs) {
    return;
  }

  if (g_waitForIrQuiet) {
    if (ir_has_activity()) {
      g_lastIrActivityMs = now;
      return;
    }
    if ((now - g_lastIrActivityMs) < LEARNING_QUIET_BEFORE_ARM_MS) {
      return;
    }
    g_waitForIrQuiet = false;
    return;
  }

  LearnedIRData captured = {};
  if (ir_poll_received(&captured)) {
    g_pendingLearnedData = captured;
    g_hasPendingLearnedData = true;
    g_pendingLearnedSinceMs = now;
    g_learningModeSinceMs = now;

    Serial.println(F("Learning: captured frame, press keypad 1-16 to save"));
    ir_print_data(g_pendingLearnedData);
  }
}

void updateLearningTimeouts() {
  if (g_mode != MODE_LEARNING) {
    return;
  }

  uint32_t now = millis();

  if (!g_hasPendingLearnedData) {
    if ((now - g_learningModeSinceMs) >= LEARNING_IDLE_TIMEOUT_MS) {
      Serial.println(F("Learning timeout: no IR received"));
      enterSendMode();
    }
    return;
  }

  if ((now - g_pendingLearnedSinceMs) >= LEARNING_ASSIGN_TIMEOUT_MS) {
    Serial.println(F("Learning timeout: no keypad button selected, captured frame dropped"));
    g_hasPendingLearnedData = false;
    memset(&g_pendingLearnedData, 0, sizeof(g_pendingLearnedData));
    g_learningModeSinceMs = now;
  }
}

} // namespace

void setup() {
  // Disable JTAG to free PA15, PB3, PB4 for GPIO use
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  Serial.setTx(PA9);
  Serial.setRx(PA10);
  Serial.begin(115200);

  initButtons();
  initLeds();
  init_ir();

  if (!storage_init()) {
    Serial.println(F("Storage init failed"));
  } else {
    Serial.println(F("Storage ready"));
  }

  Serial.print(F("Mode -> "));
  Serial.println(modeName(g_mode));
  printProfile();
}

void loop() {
  check_buttons();

  int pressed = take_button_pressed_event();
  if (pressed != 0) {
    processPressEvent(pressed);
  }

  int released = take_button_released_event();
  if (released != 0) {
    processReleaseEvent(released);
  }

  updateLearningReceiver();
  updateLearningTimeouts();
  updateHoldRepeat();

  led_update_status(g_activeProfile, g_mode == MODE_LEARNING, g_hasPendingLearnedData);
}

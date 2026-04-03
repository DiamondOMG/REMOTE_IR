#include "sweep_mode.h"
#include "ir_module.h" 
#include "learn_mode.h" // For set_system_mode
#include "profile_manager.h"
#include "storage_module.h"
#include "led_module.h"

// New SWEEP_PROTOCOLS array using specific IDs from the library dump.
// Ordered from most popular down, with the generic ones fixed at the end.
static const uint8_t SWEEP_PROTOCOLS[] = {
    // --- TOP POPULAR PROTOCOLS ---
    7,  // NEC
    8,  // NEC2
    19, // Samsung
    6,  // LG
    23, // Sony
    22, // Sharp
    10, // Panasonic
    3,  // Apple
    
    // --- SECONDARY POPULAR / NICHE ---
    4,  // Denon
    5,  // JVC
    9,  // Onkyo
    11, // Kaseikyo
    12, // Kaseikyo_Denon
    13, // Kaseikyo_Sharp
    14, // Kaseikyo_JVC
    15, // Kaseikyo_Mitsubishi
    16, // RC5
    17, // RC6
    18, // RC6A
    20, // SamsungLG
    21, // Samsung48
    24, // Bang&Olufsen
    25, // BoseWave
    26, // Lego
    27, // MagiQuest
    28, // Whynter
    29, // Marantz
    30, // FAST

    // --- FIXED LAST 3 (Generic/Unknown) ---
    0,  // UNKNOWN
    1,  // PulseWidth
    2   // PulseDistance
};
static const int NUM_SWEEP_PROTOCOLS = sizeof(SWEEP_PROTOCOLS) / sizeof(SWEEP_PROTOCOLS[0]);

// Finds the index of NEC to act as default
static int DEFAULT_PROTOCOL_INDEX = 0; // Default to index 0 (NEC)

// Current Sweep State
static int current_protocol_index = 0; // Initialize to 0 immediately
static uint16_t current_command = 0;
static uint16_t current_address = 0; // Using address 0x00 as default target

void sweep_mode_init() {
    // Dynamically find NEC index just in case array changes
    for (int i = 0; i < NUM_SWEEP_PROTOCOLS; i++) {
        if (SWEEP_PROTOCOLS[i] == 7) { // 7 is NEC in this version
            DEFAULT_PROTOCOL_INDEX = i;
            break;
        }
    }
}

void sweep_mode_enter() {
    set_system_mode(MODE_SWEEP);
    current_protocol_index = DEFAULT_PROTOCOL_INDEX;
    current_command = 0;
    current_address = 0;
    
    Serial.println(F(">>> ENTERED SWEEP MODE <<<"));
    Serial.print(F("Protocol ID: "));
    Serial.print(SWEEP_PROTOCOLS[current_protocol_index]);
    Serial.println(F(" | Press S12(UP)/S8(DOWN) to scan, S1-S12 to Save, S16 to Exit."));
}

LearnedIRData sweep_mode_get_current_signal() {
    LearnedIRData data;
    data.protocol = SWEEP_PROTOCOLS[current_protocol_index];
    data.address = current_address;
    data.command = current_command;
    
    // Assign generic bit lengths (IRremote handles most inside send functions if using modern API)
    // We just provide generic lengths, or rely on IRremote library defaults
    // Simplified bit length logic since we don't have the enums here
    // simplified bit logic - IRremote usually handles it when sending via decode_type_t
    // Assign generic bit lengths using our new IDs
    if (data.protocol == 7 || data.protocol == 8) data.numberOfBits = 32;      // NEC / NEC2
    else if (data.protocol == 19 || data.protocol == 3) data.numberOfBits = 32; // Samsung / Apple
    else if (data.protocol == 23) data.numberOfBits = 12;                      // Sony
    else if (data.protocol == 6 || data.protocol == 20) data.numberOfBits = 28; // LG / SamsungLG
    else if (data.protocol == 10) data.numberOfBits = 48;                      // Panasonic
    else data.numberOfBits = 32; // Fallback
    
    data.flags = 0; // IRDATA_FLAGS_EMPTY
    data.valid = 1;
    return data;
}

static void sweep_visual_blink(int times, int ms) {
    uint8_t mask = get_current_profile_mask();
    for (int i = 0; i < times; i++) {
        set_profile_led(0);    // All off
        delay(ms);
        set_profile_led(mask); // Back to profile (acts as "ON" for blink)
        if (i < times - 1) delay(ms);
    }
}

static void sweep_fire_signal() {
    LearnedIRData data = sweep_mode_get_current_signal();
    
    Serial.print(F("Sweeping -> Protocol ID: "));
    Serial.print(data.protocol);
    Serial.print(F(" | Addr: 0x"));
    Serial.print(data.address, HEX);
    Serial.print(F(" | Cmd: 0x"));
    Serial.println(data.command, HEX);

    // Fire IR
    ir_send_frame(data);

    // Minor visual feedback for ANY command sent
    sweep_visual_blink(1, 40); 
}

void sweep_mode_handle_button(int btn) {
    if (btn == 16) {
        // Exit Sweep Mode
        set_system_mode(MODE_SEND);
        Serial.println(F(">>> EXITED SWEEP MODE <<<"));
        return;
    }

    if (btn == 12) {
        // UP: Increase command
        if (current_command >= 0xFF) {
            current_command = 0;
            current_protocol_index++;
            if (current_protocol_index >= NUM_SWEEP_PROTOCOLS) {
                current_protocol_index = 0; // Wrap around to first
            }
            Serial.println(F("--- Moved to NEXT Protocol ---"));
            sweep_visual_blink(3, 80); // Triple blink for protocol switch
        } else {
            current_command++;
        }
        sweep_fire_signal();
        return;
    }

    if (btn == 8) {
        // DOWN: Decrease command
        if (current_command == 0) {
            current_command = 0xFF;
            current_protocol_index--;
            if (current_protocol_index < 0) {
                current_protocol_index = NUM_SWEEP_PROTOCOLS - 1; // Wrap to last
            }
            Serial.println(F("--- Moved to PREV Protocol ---"));
            sweep_visual_blink(3, 80); // Triple blink
        } else {
            current_command--;
        }
        sweep_fire_signal();
        return;
    }

    if (btn == 4) {
        // Optional: Manual Protocol switch
        current_command = 0;
        current_protocol_index++;
        if (current_protocol_index >= NUM_SWEEP_PROTOCOLS) {
            current_protocol_index = 0;
        }
        Serial.print(F("--- Quick Switched Protocol ID to: "));
        Serial.print(SWEEP_PROTOCOLS[current_protocol_index]);
        Serial.println(F(" ---"));
        return;
    }

    // It's a Data button (1-3, 5-7, 9-11, 13-15) -> SAVE
    if (is_data_button(btn)) {
        int8_t slot = button_to_slot(btn);
        if (slot >= 0) {
            LearnedIRData dataToSave = sweep_mode_get_current_signal();
            
            storage_save(slot, dataToSave);
            storage_save_profile(get_current_profile());
            
            Serial.print(F(">>> SWEEP ASSIGNED to Profile "));
            Serial.print(get_current_profile());
            Serial.print(F(", Button "));
            Serial.println(btn);
            
            // Provide a quick feedback that it saved successfully
            sweep_visual_blink(2, 60); 
        }
    }
}

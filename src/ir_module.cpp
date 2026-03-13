#include "ir_module.h"

#define IR_RECEIVE_PIN_FOR_ARDUINO IR_RECEIVE_PIN
#define IR_SEND_PIN_FOR_ARDUINO IR_SEND_PIN

#include <IRremote.hpp>

static IRData toIRData(const LearnedIRData &data, bool repeatOnly) {
    IRData sendData = {};
    sendData.protocol = static_cast<decode_type_t>(data.protocol);
    sendData.address = data.address;
    sendData.command = data.command;
    sendData.numberOfBits = data.numberOfBits;

    // Keep only direction/toggle relevant bits from saved frame.
    sendData.flags = (data.flags & (IRDATA_FLAGS_TOGGLE_BIT | IRDATA_FLAGS_IS_MSB_FIRST));
    if (repeatOnly) {
        sendData.flags |= IRDATA_FLAGS_IS_REPEAT;
    }
    return sendData;
}

void init_ir() {
    Serial.println(F("START : init_ir (Send & Receive)"));

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    IrSender.begin();

    Serial.print(F("IR Receiver: "));
    Serial.print(IR_RECEIVE_PIN);
    Serial.print(F(", IR Sender: "));
    Serial.print(IR_SEND_PIN);
    Serial.println();
}

bool ir_poll_received(LearnedIRData *outData) {
    if (outData == nullptr) {
        return false;
    }

    if (!IrReceiver.decode()) {
        return false;
    }

    const IRData &rx = IrReceiver.decodedIRData;

    bool isRepeat = (rx.flags & IRDATA_FLAGS_IS_REPEAT) != 0;
    bool isUnknown = rx.protocol == UNKNOWN;
    bool hasBits = rx.numberOfBits != 0;

    if (!isRepeat && !isUnknown && hasBits) {
        outData->valid = 1;
        outData->protocol = static_cast<uint8_t>(rx.protocol);
        outData->address = rx.address;
        outData->command = rx.command;
        outData->numberOfBits = rx.numberOfBits;
        outData->flags = rx.flags;

        IrReceiver.resume();
        return true;
    }

    IrReceiver.resume();
    return false;
}

bool ir_has_activity() {
    if (!IrReceiver.decode()) {
        return false;
    }

    IrReceiver.resume();
    return true;
}

bool ir_send_frame(const LearnedIRData &data) {
    if (!data.valid) {
        return false;
    }

    IRData sendData = toIRData(data, false);
    return IrSender.write(&sendData, 0) > 0;
}

bool ir_send_repeat(const LearnedIRData &data) {
    if (!data.valid) {
        return false;
    }

    // Ask IRremote to send protocol-specific repeat frame.
    IRData repeatData = toIRData(data, true);
    if (IrSender.write(&repeatData, 0) > 0) {
        return true;
    }

    // Fallback for protocols without special repeat implementation.
    IRData fullData = toIRData(data, false);
    return IrSender.write(&fullData, 0) > 0;
}

void ir_print_data(const LearnedIRData &data) {
    if (!data.valid) {
        Serial.println(F("IR data invalid"));
        return;
    }

    Serial.println(F("--- IR Data ---"));
    Serial.print(F("Protocol: "));
    Serial.println(getProtocolString(static_cast<decode_type_t>(data.protocol)));
    Serial.print(F("Address: 0x"));
    Serial.println(data.address, HEX);
    Serial.print(F("Command: 0x"));
    Serial.println(data.command, HEX);
    Serial.print(F("Bits: "));
    Serial.println(data.numberOfBits);
    Serial.print(F("Flags: 0x"));
    Serial.println(data.flags, HEX);
}

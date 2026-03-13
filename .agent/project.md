# 1. Project Overview
IR Learning Remote Control (STM32)
โปรเจครีโมทคอนโทรล IR ที่สามารถเรียนรู้สัญญาณจากรีโมตจริง  
บันทึกลง Flash memory และยิงสัญญาณแทนรีโมตเดิมได้  
รองรับหลายรีโมต หลายปุ่ม พร้อมแสดงสถานะด้วย LED

## Features
- รับสัญญาณ IR จากรีโมตจริง
- ส่งสัญญาณ IR ไปยังอุปกรณ์ (TV / Device)
- Learning mode สำหรับบันทึกสัญญาณ IR
- บันทึกข้อมูล IR ลงใน Flash memory (Flash storage)
- ส่งข้อมูล IR จาก Flash memory
- รองรับหลายรีโมต (เริ่มต้น 5 รีโมต)
- รีโมตละสูงสุด 16 ปุ่ม (Keypad 4x4)
- เปลี่ยนโหมด Learning / Send
- เปลี่ยน Remote profile
- แสดงสถานะการทำงานด้วย LED หลายสี

---

# 2. Hardware Specs
## MCU Overview
- **MCU:** STM32F103C6T6 (Bluepill F103X6)
- **Core:** ARM Cortex-M3 (72 MHz max)
- **Flash:** 32 KB (Verified)
- **SRAM:** 10 KB
- **Voltage:** 2.0V - 3.6V (Standard 3.3V)

## Peripherals / Components
- IR Receiver (38kHz demodulated)
- IR LED + Transistor driver
- ปุ่มกดแบบคงที่ 2 ปุ่ม (เปลี่ยนโหมด และ เปลี่ยน Remote)
- Matrix Keypad 4x4 (16 ปุ่ม)
- LED แสดงสถานะ 5 ดวง (แดง, ขาว, เหลือง, เขียว, น้ำเงิน)

## ⚠️ Hardware Limits
- **Timers:** 3x General Purpose (TIM2, TIM3, TIM4) + 1x Advanced (TIM1)
- **DMA:** 7 Channels (ช่วยลดภาระ CPU ในการย้ายข้อมูล)
- **Flash Memory:** ค่อนข้างจำกัดที่ 32KB ต้องจัดการให้ดี
- **ADC Warning:** ห้ามจ่ายไฟเกิน 3.3V เข้าขา ADC เด็ดขาด (ขาเหล่านี้ไม่ 5V-Tolerant)

---

# 3. Pin Mapping Table
*(ต้องอัปเดตเมื่อมีการกำหนดขาในโปรเจกต์)*

| Peripheral / Device | Pin | Type / Config | Description / Function |
|---|---|---|---|
| **UART1 (Serial)** | PA9 (TX), PA10 (RX) | 5V Tolerant | สำหรับสื่อสารกับ Computer (Upload/Log) |
| **Built-in LED** | PC13 | Active LOW | LED บนบอร์ด |
| *(อื่นๆ รอการกำหนด)* | | | |

---

# 4. Library Specs
- (รอตรวจสอบ/เพิ่ม Library เช่น IRremote, Keypad)

---

# 5. Config & Settings
## PlatformIO (`platformio.ini`)
```ini
platform = ststm32
board = bluepill_f103c6
framework = arduino
upload_protocol = stlink
debug_tool = stlink
monitor_speed = 115200
```

## System Rules / Settings
- **IR Data Format:** เก็บแบบ Decoded IR (Protocol, Address, Command, Num of bits) ไม่ส่ง IR repeat ถ้าไม่จำเป็น
- **LED Modes:**
  - Remote active: ไฟค้าง
  - Learning mode: ไฟกระพริบช้า
  - รับ IR ได้: ไฟกระพริบรัว
- **Timeout / Safety:** ไม่รับ IR ในเวลาที่กำหนดจะตกเป็น Timeout / เขียน Flash เท่าที่จำเป็นเพื่อลด Wear

---

# 6. Current State
- ✅ สร้างโฟลเดอร์ `.agent` 
- ✅ สร้างระบบ Workflow Upload / Monitor
- ✅ Setup `platformio.ini` และ `.gitignore`
- 🎯 สิ่งที่ต้องทำต่อไป: จัดเตรียมโครงสร้างไฟล์ Source Code (`main.cpp`, `module.h`, `module.cpp`) และไลบรารี

---

# 7. Issues & Solutions Log
| ปัญหา | สาเหตุ | วิธีแก้ปัญหาอย่างย่อ | Date / Status |
| --- | --- | --- | --- |
| การอัปโหลดไม่ผ่าน | ไม่ได้เข้า Bootloader | ต้องเซ็ตหัวจัมพ์ **Boot0=1, Boot1=0** และกดปุ่ม Reset 1 ครั้งก่อนการแฟลชโค้ด | - |

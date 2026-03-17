# 📁 Project: IR Remote (STM32F103C6T6)

## 🎯 1. Project Overview
โปรเจกต์สร้างรีโมทพกพา (Universal Remote) ที่สามารถเรียนรู้ (Learn) และส่งสัญญาณ (Send) IR ได้หลายโปรไฟล์ โดยใช้บอร์ด STM32F103C6T6 เป็นตัวประมวลผลหลัก

**ฟีเจอร์หลัก:**
- รองรับ 31 โปรไฟล์ (แสดงผลผ่าน LED 5 สี)
- Button Matrix 4x4 (12 ปุ่มใช้งาน + 4 ปุ่มระบบ)
- โหมดส่งสัญญาณ (Send Mode) และโหมดเรียนรู้ (Learn Mode)
- เก็บข้อมูลลงหน่วยความจำ Flash (Internal Flash Emulated EEPROM)

## 🔌 2. Hardware Specs & Library
(อ้างอิงจาก [hardware_lib.md](file:///c:/MCU/STM32/STM32_F103_C6T6/REMOTE_IR/hardware_lib.md))

### MCU: STM32F103C6T6 (Bluepill)
- Flash: 32 KB
- SRAM: 10 KB

### Peripherals:
- **Button Matrix 4x4**: สำหรับควบคุมและป้อนข้อมูล
- **LED 5 สี**: แดง, เหลือง, เขียว, น้ำเงิน, ขาว (ใช้แสดงผล Profile / Status)
- **IR LED Send**: สำหรับส่งสัญญาณ
- **IR Receiver**: สำหรับรับและเรียนรู้สัญญาณ
- **Power System**: รางถ่าน 1.5V + Boost Converter to 5V

### Library Specs:
- **IRremote**: `z3t0/IRremote@^4.3.1` (ใช้สำหรับจัดการสัญญาณ IR)

## 📌 3. Pin Mapping Table
*(รอยืนยันขา Pin ที่แน่นอนในขั้นตอนการสร้าง Module)*

| Device | Pin | Function | Description |
|---|---|---|---|
| IR Send | PA8 | PWM/Output | ขาส่งสัญญาณ IR (TIM1) |
| IR Recv | PA11 | Input | ขารับสัญญาณ IR |
| LED Red | PA0 | Output | |
| LED Yellow | PA2 | Output | |
| LED Green | PA4 | Output | |
| LED Blue | PA6 | Output | |
| LED White | PB0 | Output | |
| Matrix Row 1-4 | PA15, PB3, PB4, PB5 | Input/Pull | Rows |
| Matrix Col 1-4 | PB6, PB7, PB8, PB9 | Output | Columns |

## ⚙️ 4. Config & Settings
- **Upload Protocol**: UART Serial (USART1: PA9, PA10)
- **Baud Rate**: 115200 (Monitor & Upload)
- **Profiles**: 31 Profiles (5-bit binary display via LEDs)
- **System Buttons (Locked)**: 
    - ปุ่ม 4: สลับโหมด (Learn/Send)
    - ปุ่ม 8: เลื่อนโปรไฟล์ขึ้น
    - ปุ่ม 12: เลื่อนโปรไฟล์ลง
    - ปุ่ม 16: ทดสอบสัญญาณ (Test Send)

## 🚀 5. Current State
- [x] ออกแบบคอนเซปต์โปรเจกต์
- [x] เตรียมไฟล์ `rule.md` และ `hardware_lib.md`
- [x] สร้างโครงโปรเจกต์ใน PlatformIO
- [ ] สร้าง `main.cpp`
- [ ] ทดสอบโมดูล LED และ Button Matrix

## 📝 6. Issues & Solutions Log
- ยังไม่มีประวัติปัญหา

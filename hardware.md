# 📟 MCU Specification: STM32F103C6 (Bluepill)

### 📋 General Specs
- **Core:** ARM Cortex-M3
- **Max Clock:** 72 MHz
- **Default Flash:** 32 KB (Verified)
- **SRAM:** 10 KB
- **Voltage:** 2.0V - 3.6V (Standard 3.3V)

### 🔌 Detailed Pinout & Peripheral Reference
| Peripheral | Pins | 5V Tolerant? | Description |
|---|---|---|---|
| **UART1** | PA9 (TX), PA10 (RX) | **Yes** | Main Serial (Used for Upload/Log) |
| **UART2** | PA2 (TX), PA3 (RX) | **Yes** | Secondary Serial |
| **I2C1** | PB6 (SCL), PB7 (SDA) | **Yes** | Standard I2C Port |
| **SPI1** | PA5 (SCK), PA6 (MISO), PA7 (MOSI) | No | High Speed Serial Peripheral |
| **ADC1/2** | PA0 - PA7, PB0, PB1 | No | 12-bit Analog Input (Max 3.3V) |
| **USB** | PA11 (D-), PA12 (D+) | **Yes** | USB Full Speed |
| **LED** | PC13 | No | Built-in LED (Active LOW) |

### ⚙️ Hardware Limits (F103X6)
- **Flash:** 32 KB
- **RAM:** 10 KB
- **Timers:** 3x General Purpose (TIM2, TIM3, TIM4) + 1x Advanced (TIM1)
- **DMA:** 7 Channels (ช่วยลดภาระ CPU ในการย้ายข้อมูล)

### ⚙️ Recommended PlatformIO Config
```ini
platform = ststm32
board = bluepill_f103c6
framework = arduino
upload_protocol = serial
monitor_speed = 115200
```

### ⚠️ Common Hardware Issues / Tips
- **Bootloader Mode (Mandatory for UART):** 
    1. ปรับ Jumper: **Boot0 = 1, Boot1 = 0**
    2. **กดปุ่ม Reset** บนบอร์ด (เพื่อเข้าสู่ Bootloader Mode)
    3. จึงจะเริ่มสั่ง Flash โค้ดได้
- **After Flash:** หากต้องการให้โค้ดรันอัตโนมัติหลัง Reset ครั้งถัดไป ต้องปรับ **Boot0 = 0**
- **Wait for Serial:** เมื่อใช้ `pio run -t upload -t monitor` ควรใส่ `delay(6000)` ใน `setup()` เสมอ
- **ADC Warning:** ห้ามจ่ายไฟเกิน 3.3V เข้าขา ADC เด็ดขาด (ขาเหล่านี้ไม่ 5V-Tolerant)

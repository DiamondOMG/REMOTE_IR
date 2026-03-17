---
description: Build and upload code to MCU using PlatformIO
---

# Upload Firmware

**⚠️ สำคัญ:**
1. ปรับ Jumper: **Boot0 = 1, Boot1 = 0**
2. **กดปุ่ม Reset** บนบอร์ด (เพื่อเข้าสู่ Bootloader Mode)
3. ตรวจสอบการเชื่อมต่อ USB-to-TTL (TX -> PA10, RX -> PA9, GND -> GND)

// turbo
```bash
pio run -t upload
```

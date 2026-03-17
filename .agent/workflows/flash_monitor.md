---
description: Build, upload and open serial monitor
---

# Flash & Monitor

**⚠️ สำคัญก่อนรันคำสั่ง:**
1. ปรับ Jumper: **Boot0 = 1, Boot1 = 0**
2. **กดปุ่ม Reset** บนบอร์ด (เพื่อเข้าสู่ Bootloader Mode)
3. ตรวจสอบการเชื่อมต่อ USB-to-TTL (TX -> PA10, RX -> PA9, GND -> GND)
4. ต้องมี `delay(6000)` ใน `setup()` ตอนเริ่มต้น เพื่อไม่ให้ข้อความ Serial หายขณะเปิด Monitor

// turbo
```bash
pio run -t upload -t monitor
```

---
description: Build, upload and open serial monitor
---

# Flash & Monitor

**⚠️ สำคัญก่อนรันคำสั่ง:**
1. โปรดเช็คการเชื่อมต่อสาย ST-Link (SWDIO, SWCLK, GND, 3.3V) ให้ถูกต้อง
2. แนะนำให้มี `delay(2000)` ใน `setup()` ตอนเริ่มต้น เพื่อไม่ให้ข้อความ Serial หายขณะเปิด Monitor

// turbo
```bash
pio run -t upload -t monitor
```

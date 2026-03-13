#include "remote_storage.h"
#include <string.h>

// สำหรับ Official STM32 Core (STMicroelectronics)
#include "stm32f1xx_hal.h"

namespace {

static const uint32_t STORAGE_MAGIC = 0x314C5249UL; // "IRL1"
static const uint8_t STORAGE_VERSION = 2;

// พื้นที่ Flash สำหรับ STM32F103 (32KB หรือ 64KB)
// เราจะใช้พื้นที่ท้ายๆ ของ 32KB เริ่มที่ Page 26 (0x08006800)
static const uint32_t FLASH_STORAGE_BASE = 0x08006800; // Page 26
// ลบ FLASH_PAGE_SIZE ออกเพราะซ้ำกับระบบ

struct StorageHeader {
    uint32_t magic;
    uint8_t version;
    uint8_t profiles;
    uint8_t buttons;
    uint8_t reserved;
    uint16_t checksum;
};

uint32_t getSlotAddress(uint8_t profileIndex, uint8_t buttonIndex) {
    uint32_t offset = sizeof(StorageHeader) + 
                      (static_cast<uint32_t>(profileIndex) * REMOTE_BUTTON_COUNT * sizeof(LearnedIRData)) +
                      (static_cast<uint32_t>(buttonIndex - 1) * sizeof(LearnedIRData));
    return FLASH_STORAGE_BASE + offset;
}

} // namespace

bool storage_init() {
    const StorageHeader* header = reinterpret_cast<const StorageHeader*>(FLASH_STORAGE_BASE);
    
    if (header->magic != STORAGE_MAGIC || header->version != STORAGE_VERSION) {
        Serial.println(F("Flash storage invalid. Formatting..."));
        
        HAL_FLASH_Unlock();
        
        // ล้างพื้นที่ (Erase) 6 หน้า
        FLASH_EraseInitTypeDef eraseInit;
        eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        eraseInit.PageAddress = FLASH_STORAGE_BASE;
        eraseInit.NbPages = 6;
        uint32_t pageError = 0;
        
        if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }

        // เขียน Header ใหม่
        StorageHeader newHeader = { STORAGE_MAGIC, STORAGE_VERSION, REMOTE_PROFILE_COUNT, REMOTE_BUTTON_COUNT, 0, 0 };
        uint64_t* headerData = reinterpret_cast<uint64_t*>(&newHeader);
        
        // STM32 HAL เขียนด้วย DoubleWord (64-bit) หรือ HalfWord ก็ได้ 
        // ในที่นี้เขียนทีละ 2 bytes (HalfWord) เพื่อความง่าย
        uint16_t* ptr = reinterpret_cast<uint16_t*>(&newHeader);
        for (uint32_t i = 0; i < sizeof(StorageHeader); i += 2) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_STORAGE_BASE + i, *ptr++);
        }
        
        HAL_FLASH_Lock();
        Serial.println(F("Flash formatted."));
    }
    return true;
}

bool storage_read_slot(uint8_t profileIndex, uint8_t buttonIndex, LearnedIRData *outData) {
    if (profileIndex >= REMOTE_PROFILE_COUNT || buttonIndex < 1 || buttonIndex > REMOTE_BUTTON_COUNT) {
        return false;
    }
    uint32_t addr = getSlotAddress(profileIndex, buttonIndex);
    memcpy(outData, reinterpret_cast<const void*>(addr), sizeof(LearnedIRData));
    return outData->valid != 0;
}

bool storage_write_slot(uint8_t profileIndex, uint8_t buttonIndex, const LearnedIRData &data) {
    if (profileIndex >= REMOTE_PROFILE_COUNT || buttonIndex < 1 || buttonIndex > REMOTE_BUTTON_COUNT) {
        return false;
    }

    uint32_t targetAddr = getSlotAddress(profileIndex, buttonIndex);
    if (memcmp(reinterpret_cast<const void*>(targetAddr), &data, sizeof(LearnedIRData)) == 0) {
        return true;
    }

    // จัดตำแหน่ง Page Address ให้ถูกต้อง
    uint32_t pageAddr = (targetAddr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
    uint32_t offsetInPage = targetAddr - pageAddr;

    // Buffer 1KB ใน RAM (ชั่วคราว)
    uint8_t pageBuffer[FLASH_PAGE_SIZE];
    memcpy(pageBuffer, reinterpret_cast<const void*>(pageAddr), FLASH_PAGE_SIZE);
    memcpy(pageBuffer + offsetInPage, &data, sizeof(LearnedIRData));

    HAL_FLASH_Unlock();
    
    // Erase 1 page ก่อนเขียนใหม่
    FLASH_EraseInitTypeDef eraseInit;
    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = pageAddr;
    eraseInit.NbPages = 1;
    uint32_t pageError = 0;
    
    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    // เขียนข้อมูลทั้งหน้ากลับลงไป
    uint16_t* ptr = reinterpret_cast<uint16_t*>(pageBuffer);
    for (uint32_t i = 0; i < FLASH_PAGE_SIZE; i += 2) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, pageAddr + i, *ptr++);
    }

    HAL_FLASH_Lock();
    return true;
}

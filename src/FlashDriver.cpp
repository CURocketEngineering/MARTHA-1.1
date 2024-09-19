#include "FlashDriver.h"

 // FLASH_CS is the Chip Select pin
 // 100000 is the clock speed (100kHz)
 // SPI_BITORDER_MSBFIRST sets the bit order
 // SPI_MODE1 sets the SPI mode
FlashDriver::FlashDriver() : flashSpi(FLASH_CS, 100000, SPI_BITORDER_MSBFIRST, SPI_MODE1) {}

FlashDriver::~FlashDriver() {}

Status FlashDriver::init() {
    if (!flashSpi.begin()) {
        return FAILURE;
    }

    digitalWrite(FLASH_CS, LOW);
    flashSpi.beginTransaction();
    flashSpi.transfer(0x9F);  

     // Read Manufacturer ID and Device ID
    uint8_t manufacturerID = flashSpi.transfer(0x00);  // Read Manufacturer ID
    uint8_t memoryType = flashSpi.transfer(0x00);      // Read Memory Type (should be 0x40 for W25Q128JV)
    uint8_t capacity = flashSpi.transfer(0x00);        // Read Memory Capacity (should be 0x18 for W25Q128JV)


    flashSpi.endTransaction();
    digitalWrite(FLASH_CS, HIGH);

    if (manufacturerID == 0xEF && memoryType == 0x40 && capacity == 0x18) {
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

Status FlashDriver::read(uint32_t address, uint8_t* buffer, size_t length) {
    if (!buffer || length == 0) {
        return INVALID;
    }

    digitalWrite(FLASH_CS, LOW);
    flashSpi.beginTransaction();
    flashSpi.transfer(0x03);                    // Send the Read command (0x03)    
    flashSpi.transfer((address >> 16) & 0xFF);
    flashSpi.transfer((address >> 8) & 0xFF);
    flashSpi.transfer(address & 0xFF);

    for (size_t i = 0; i < length; i++) {
        buffer[i] = flashSpi.transfer(0x00);
    }

    flashSpi.endTransaction();
    digitalWrite(FLASH_CS, HIGH);

    return SUCCESS;
}

void FlashDriver::writeDisable() {
    digitalWrite(FLASH_CS, LOW);
    flashSpi.beginTransaction();
    flashSpi.transfer(0x04);                     // Send the Write Diable command (0x04)
    flashSpi.endTransaction();
    digitalWrite(FLASH_CS, HIGH);
}

Status FlashDriver::write(uint32_t address, const uint8_t* data, size_t length) {
    if (!data || length == 0) {
        return INVALID;
    }

    digitalWrite(FLASH_CS, LOW);
    flashSpi.beginTransaction();
    flashSpi.transfer(0x06);                      // Send the Write command (0x06)
    flashSpi.transfer((address >> 16) & 0xFF);
    flashSpi.transfer((address >> 8) & 0xFF);
    flashSpi.transfer(address & 0xFF);

    for (size_t i = 0; i < length; i++) {
        flashSpi.transfer(data[i]);
    }

    flashSpi.endTransaction();
    digitalWrite(FLASH_CS, HIGH);

    writeDisable();

    return SUCCESS;
}





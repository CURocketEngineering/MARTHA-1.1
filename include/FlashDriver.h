#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include "Adafruit_SPIDevice.h"

#define FLASH_CS 10

enum Status {
    SUCCESS,
    FAILURE,
    INVALID
};

class FlashDriver {
    Adafruit_SPIDevice flashSpi;

    public:
    FlashDriver();  // Constructor
    ~FlashDriver(); // Destructor

    Status init();
    Status read(uint32_t address, uint8_t* buffer, size_t length);
    Status write(uint32_t address, const uint8_t* data, size_t length);
    
    private:
    void readEnable();
    void writeEnable(); 
    void writeDisable();
};

#endif // FLASH_DRIVER_H


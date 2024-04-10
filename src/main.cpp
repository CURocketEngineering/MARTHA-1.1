#include <Arduino.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_LSM6DSOX.h>

#include "SensorDataHandler.h"
#include <SD.h>

#define DEBUG Serial

Adafruit_MPL3115A2 baro;
Adafruit_LSM6DSOX sox;

// ----------------
// Initalization of data handlers
// ----------------
// First parameter is the interval between each data point in milliseconds
// Second parameter is the size of the temporal array in milliseconds (i.e. hold old data is the oldest data point)
// Third parameter is the name of the data
SensorData altitudeData(62.5, 5000, "at"); // 62.5ms is the interval between each data point

SensorData xAccelData(62.5, 3000, "xa");
SensorData yAccelData(62.5, 3000, "ya");
SensorData zAccelData(200, 7000, "za");

SensorData xGyroData(62.5, 1000, "xg");
SensorData yGyroData(62.5, 1000, "yg");
SensorData zGyroData(62.5, 1000, "zg");

// Storing these at a slower rate b/c less important
SensorData temperatureData(500, 1000, "tp"); 
SensorData pressureData(500, 1000, "ps"); 

// For the serial SD card logger
HardwareSerial SD_serial(PB7, PB6); // RX, TX

int last_led_toggle = 0;

void setup(void) {
  
  pinMode(PA9, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  
  SD_serial.begin(9600);
  while (!SD_serial)
    delay(10);

  Serial.println("All serial communication is setup");

  // Start the SPI SD card
  SD.begin(PA4);



  // Run this to test the data handler, will stop the program after it finishes
  // test_DataHandler();

  TwoWire *wire = new TwoWire(PB11, PB10);

  Serial.println("Setting up barometer...");

  if (!baro.begin(wire)) {
    Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }

  // Setting the barometer to altimeter
  baro.setMode(MPL3115A2_ALTIMETER);
  Serial.println("Setting up accelerometer and gyroscope...");
  if (!sox.begin_I2C(0x6A, wire)) {
    while (1) {
      delay(10);
    }
  }

  Serial.println("Setting ACL and Gyro ranges and data rates...");
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
  sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS );

  sox.setAccelDataRate(LSM6DS_RATE_104_HZ);
  sox.setGyroDataRate(LSM6DS_RATE_104_HZ);

  // If the range is not set correctly, then print a message
  if (sox.getAccelRange() != LSM6DS_ACCEL_RANGE_16_G) {
    Serial.println("Failed to set ACL range");
  }
  if (sox.getGyroRange() != LSM6DS_GYRO_RANGE_2000_DPS) {
    Serial.println("Failed to set Gyro range");
  }
  if (sox.getAccelDataRate() != LSM6DS_RATE_104_HZ) {
    Serial.println("Failed to set ACL data rate");
  }
  if (sox.getGyroDataRate() != LSM6DS_RATE_104_HZ) {
    Serial.println("Failed to set Gyro data rate");
  }
}

void loop() {
  uint32_t current_time = millis();
  if (current_time - last_led_toggle > 1000) {
    last_led_toggle = millis();
    digitalWrite(PA9, !digitalRead(PA9));
  }
  


  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  // Storing data in the data handlers
  altitudeData.addData(DataPoint(current_time, baro.getAltitude()), SD_serial);

  xAccelData.addData(DataPoint(current_time, accel.acceleration.x), SD_serial);
  yAccelData.addData(DataPoint(current_time, accel.acceleration.y), SD_serial);
  zAccelData.addData(DataPoint(current_time, accel.acceleration.z), SD_serial);

  xGyroData.addData(DataPoint(current_time, gyro.gyro.x), SD_serial);
  yGyroData.addData(DataPoint(current_time, gyro.gyro.y), SD_serial);
  zGyroData.addData(DataPoint(current_time, gyro.gyro.z), SD_serial);

  temperatureData.addData(DataPoint(current_time, temp.temperature), SD_serial);
  pressureData.addData(DataPoint(current_time, baro.getPressure()), SD_serial);


  SensorData * viewerPtr = &temperatureData;
  // Print out altitude data
  Serial.println((*viewerPtr).getLatestData().data);
}
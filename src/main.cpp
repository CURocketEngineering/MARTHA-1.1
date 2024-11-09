
#include <Arduino.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <SD.h>

#include "data_handling/SensorDataHandler.h"
#include "data_handling/DataSaverSDSerial.h"
#include "data_handling/DataNames.h"

#define DEBUG Serial

Adafruit_MPL3115A2 baro;
Adafruit_LSM6DSOX sox;
Adafruit_LIS3MDL mag;


// For the serial SD card logger
HardwareSerial SD_serial(PB7, PB6); // RX, TX
DataSaverSDSerial dataSaverSDSerial(SD_serial);

SensorDataHandler xAccelData(ACCELEROMETER_X, &dataSaverSDSerial);
SensorDataHandler yAccelData(ACCELEROMETER_Y, &dataSaverSDSerial);
SensorDataHandler zAccelData(ACCELEROMETER_Z, &dataSaverSDSerial);

SensorDataHandler xGyroData(GYROSCOPE_X, &dataSaverSDSerial);
SensorDataHandler yGyroData(GYROSCOPE_Y, &dataSaverSDSerial);
SensorDataHandler zGyroData(GYROSCOPE_Z, &dataSaverSDSerial);

// Storing these at a slower rate b/c less important
SensorDataHandler temperatureData(TEMPERATURE, &dataSaverSDSerial);

int last_led_toggle = 0;

void setup(void) {
  
  pinMode(PA9, OUTPUT);
  
  // Uncomment this to see the debug messages
  // Serial.begin(115200);
  // while (!Serial)
  //   delay(10); // will pause Zero, Leonardo, etc until serial console opens


  SD_serial.begin(115200);
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
    while (1);
  }

  baro.setMode(MPL3115A2_ALTIMETER);
  baro.setSeaPressure(240); // Adjust this to your local forecast!

  temperatureData.restrictSaveSpeed(1000); // Save temperature data every second

  // Setting the barometer to altimeter
  // baro.setMode(MPL3115A2_ALTIMETER);
  Serial.println("Setting up accelerometer and gyroscope...");
  while (!sox.begin_I2C(0x6A, wire)) {
    Serial.println("Could not find sensor. Check wiring.");
    delay(10);
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

  // Setup for the magnetometer
  // Serial.println("Setting up magnetometer...");
  // while (!mag.begin_I2C(0x1E, wire)) {
  //   Serial.println("Could not find sensor. Check wiring.");
  //   delay(10);
  // }
  // mag.setDataRate(LIS3MDL_DATARATE_155_HZ);
  // mag.setRange(LIS3MDL_RANGE_4_GAUSS);
  // mag.setOperationMode(LIS3MDL_SINGLEMODE);
  // mag.setPerformanceMode(LIS3MDL_MEDIUMMODE);

  // mag.setIntThreshold(500);
  // mag.configInterrupt(false, false, true, // enable z axis
  //                         true, // polarity
  //                         false, // don't latch
  //                         true); // enabled!

  // if (mag.getDataRate() != LIS3MDL_DATARATE_155_HZ) {
  //   Serial.println("Failed to set Mag data rate");
  // }
  // test_DataHandler();
  temperatureData.restrictSaveSpeed(1000); // Save temperature data every second
  Serial.println("Setup Complete!!!");
}

void loop() {
  int toggle_delay = 500;
  // if (flightStatus.getStage() > ARMED) {
  //   toggle_delay = 50;
  // }
  uint32_t current_time = millis();
  if (current_time - last_led_toggle > toggle_delay) {
    last_led_toggle = millis();
    digitalWrite(PA9, !digitalRead(PA9));
  }

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  xAccelData.addData(DataPoint(current_time, accel.acceleration.x));
  yAccelData.addData(DataPoint(current_time, accel.acceleration.y));
  zAccelData.addData(DataPoint(current_time, accel.acceleration.z));

  xGyroData.addData(DataPoint(current_time, gyro.gyro.x));
  yGyroData.addData(DataPoint(current_time, gyro.gyro.y));
  zGyroData.addData(DataPoint(current_time, gyro.gyro.z));

  temperatureData.addData(DataPoint(current_time, temp.temperature));

  // flightStatus.update(&SD_serial);
}
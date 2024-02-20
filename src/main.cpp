#include <Arduino.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_LSM6DSOX.h>

#include "CRE_DataHandler.h"

#define DEBUG Serial

Adafruit_MPL3115A2 baro;
Adafruit_LSM6DSOX sox;

// ----------------
// Initalization of data handlers
// ----------------
// First parameter is the interval between each data point in milliseconds
// Second parameter is the size of the temporal array in milliseconds (i.e. hold old data is the oldest data point)
SensorData altitudeData(62.5, 5000); // 62.5ms is the interval between each data point

SensorData xAccelData(62.5, 1000);
SensorData yAccelData(62.5, 1000);
SensorData zAccelData(62.5, 1000);

SensorData xGyroData(62.5, 1000);
SensorData yGyroData(62.5, 1000);
SensorData zGyroData(62.5, 1000);

// Storing these at a slower rate b/c less important
SensorData temperatureData(500, 1000); 
SensorData pressureData(500, 1000); 

int last_led_toggle = 0;

void setup(void) {
  
  pinMode(PA9, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

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
  altitudeData.addData(DataPoint(current_time, baro.getAltitude()));

  xAccelData.addData(DataPoint(current_time, accel.acceleration.x));
  yAccelData.addData(DataPoint(current_time, accel.acceleration.y));
  zAccelData.addData(DataPoint(current_time, accel.acceleration.z));

  xGyroData.addData(DataPoint(current_time, gyro.gyro.x));
  yGyroData.addData(DataPoint(current_time, gyro.gyro.y));
  zGyroData.addData(DataPoint(current_time, gyro.gyro.z));

  temperatureData.addData(DataPoint(current_time, temp.temperature));
  pressureData.addData(DataPoint(current_time, baro.getPressure()));
}
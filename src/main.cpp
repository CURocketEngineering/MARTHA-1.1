#include <Arduino.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_LSM6DSOX.h>

#define DEBUG Serial

Adafruit_MPL3115A2 baro;
Adafruit_LSM6DSOX sox;

int last_led_toggle = 0;

void setup(void) {
  pinMode(PA9, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

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
  if (millis() - last_led_toggle > 1000) {
    last_led_toggle = millis();
    digitalWrite(PA9, !digitalRead(PA9));
  }
  


  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  // Serial.print("\t\tTemperature ");
  // Serial.print(temp.temperature);
  // Serial.println(" deg C");

  /* Display the results (acceleration is measured in m/s^2) */
  // Serial.print("\t\tAccel X: ");
  // Serial.print(accel.acceleration.x);
  // Serial.print(" \tY: ");
  // Serial.print(accel.acceleration.y);
  // Serial.print(" \tZ: ");
  // Serial.print(accel.acceleration.z);
  // Serial.println(" m/s^2 ");

  /* Display the results (rotation is measured in rad/s) */
  // Serial.print("\t\tGyro X: ");
  // Serial.print(gyro.gyro.x);
  // Serial.print(" \tY: ");
  // Serial.print(gyro.gyro.y);
  // Serial.print(" \tZ: ");
  // Serial.print(gyro.gyro.z);
  // Serial.println(" radians/s ");
  // Serial.println();

  // Baro stuff
  float pressure = baro.getPressure();
  float altitude = baro.getAltitude();
  float temperature = baro.getTemperature();

  // Serial.println("-----------------");
  // Serial.print("pressure = "); Serial.print(pressure); Serial.println(" hPa");
  // Serial.print("altitude = "); Serial.print(altitude); Serial.println(" m");
  // Serial.print("temperature = "); Serial.print(temperature); Serial.println(" C");

  Serial.println(millis());
}
#include "Adafruit_VL53L0X.h"
#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
MPU9250_asukiaaa mySensor;
float mDirection, mX, mY, mZ, baseline, minBase, maxBase, inc;
bool initialLoop = true;

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

#ifdef _ESP32_HAL_I2C_H_ // For ESP32
  Wire.begin(SDA_PIN, SCL_PIN);
  mySensor.setWire(&Wire);
#endif

  mySensor.beginMag();  
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if ((measure.RangeStatus != 4) && (mySensor.magUpdate() == 0)) {  // phase failures have incorrect data
    mZ = mySensor.magZ();
    if (initialLoop) {
      baseline = mZ; 
      //inc = baseline*0.2;
      minBase = baseline - 5;
      maxBase = baseline + 10;
    }
    Serial.print("Distance (mm): " + measure.RangeMilliMeter); 
    Serial.print(" magZ: " + String(mZ));
    Serial.println();
    if (mZ < minBase || mZ > maxBase) {
      Serial.println("Car is here");
    }
    else {
      Serial.println("cars not here");
    }
  } else {
    Serial.println(" out of range ");
    Serial.println("Cannot read mag values");
  }
    
  delay(100);
  initialLoop = false;
}

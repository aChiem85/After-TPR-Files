#include "Adafruit_VL53L0X.h"
#include <MPU9250_asukiaaa.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
MPU9250_asukiaaa mySensor;
float mDirection, mX, mY, mZ, baseMz, hiMz, loMz;
int mZCounter = 0;


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
  
  //Gives baseMz the inital value and 'stores' it.
  mySensor.magUpdate();
  baseMz = mySensor.magZ();
  Serial.println("base: " + String(baseMz));
  
  
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if ((measure.RangeStatus != 4) && (mySensor.magUpdate() == 0)) {  // phase failures have incorrect data
     //For comparison
     loMz = baseMz - 10;
     hiMz = baseMz + 10;
     
     //Magnetomter Z value
     mZ = mySensor.magZ();
     
     //Outputs of ToF, baseMz and observed mZ
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter); 
    Serial.println("base: " + String(baseMz));
    Serial.println("magZ: " + String(mZ));

//    //This is just to output a message to know the possible detections
//    if ((mZ <= loMz || mZ >= hiMz) && measure.RangeMilliMeter > 90)
//    {
//      Serial.println("Car is present..");
//    }
//    else if ((mZ <= loMz || mZ >= hiMz) && measure.RangeMilliMeter <=90)
//    {
//      Serial.println("Sense something ferrous, but nothing above..");
//    }
//    else if (measure.RangeMilliMeter > 90)
//    {
//      Serial.println("Car MIGHT be there..");
//    }
//    else
//    {
//      Serial.println("Car not present..");
//    }
//    
//    Serial.println();
//    Serial.println();
//    Serial.println();
//    Serial.println();
//  } else {
//    Serial.println(" out of range ");
//    Serial.println("Cannot read mag values");
//    Serial.println();
//    Serial.println();
//    Serial.println();
//    Serial.println();
//    Serial.println();
//    Serial.println();
//    Serial.println();
//  }
//    
  delay(250);
  }
  }

#include "Adafruit_VL53L0X.h"
#include <MPU9250_asukiaaa.h>
#include <MKRWAN.h>

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21
#define SCL_PIN 22
#endif
//
//Adafruit_VL53L0X lox = Adafruit_VL53L0X();
//MPU9250_asukiaaa mySensor;
  LoRaModem modem(Serial1);
  String msg = "Hello";

//float mDirection, mX, mY, mZ;
//bool status = false;
String appEui = "0000000000000000";
//String appKey = "4de4012bb6946228c45a7e4c935b95b8"; // nsx
//String appKey = "bd3d5ce14b9b2db692d1829abed7e6b2"; // Supra
String appKey = "35a8eef58147c60bb53666df81ee9ec3"; // Mr2

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
    if (!modem.begin(US915)) {
      Serial.println("Failed to start module");
      while (1) {}
    }
//  }
//  
//  Serial.println("Adafruit VL53L0X test");
//  if (!lox.begin()) {
//    Serial.println(F("Failed to boot VL53L0X"));
//    while(1);
//  }
  
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

//  #ifdef _ESP32_HAL_I2C_H_ // For ESP32
//    Wire.begin(SDA_PIN, SCL_PIN);
//    mySensor.setWire(&Wire);
//  #endif
//
//  mySensor.beginMag();  
//  // power 
//  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
//}
//
//
//void loop() {
//  VL53L0X_RangingMeasurementData_t measure;
//    
//  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
//
//  if ((measure.RangeStatus != 4) && (mySensor.magUpdate() == 0)) {  // phase failures have incorrect data
//    mZ = mySensor.magZ();
//    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter); 
//    Serial.print("magZ: " + String(mZ));
//    Serial.println();
//     if (mZ < 3 || mZ > 12) {
//      Serial.println("Car is here");
//    }
//    else {
//      Serial.println("cars not here");
//    }
//  }
//
//  if (status == false) {
//     if ((mZ < 3 || mZ > 12) && measure.RangeMilliMeter < 50) {
//      status = true;
//      Serial.println();
//      Serial.println("Sending 'TRUE'");
//      
//      String msg = "TRUE";
//  
      int err;
      modem.beginPacket();
      modem.print(msg);
      err = modem.endPacket(true);
      if (err > 0) {
        Serial.println("Message sent correctly!");
      } else {
        Serial.println("Error sending message :(");
        Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
        Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
      }
  }
//  } else if (status == true) {
//    delay(20000);
//    if (mZ < 0 && measure.RangeMilliMeter > 50) {
//      status = false;
//      Serial.println();
//      Serial.println("Sending 'FALSE'");
//      
//      String msg = "FALSE";
  
//      int err;
//      modem.beginPacket();
//      modem.print(msg);
//      err = modem.endPacket(true);
//      if (err > 0) {
//        Serial.println("Message sent correctly!");
//      } else {
//        Serial.println("Error sending message :(");
//        Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
//        Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
//      }
//    }
//   }
//    
  delay(1000);
}

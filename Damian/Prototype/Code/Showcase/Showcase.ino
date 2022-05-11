#include "Adafruit_VL53L0X.h" //// time of flight sensor library
#include <MPU9250_asukiaaa.h> //// magnetometer library 
#include <MKRWAN.h>            

#ifdef _ESP32_HAL_I2C_H_
#define SDA_PIN 21            //// serial Data pin https://www.electronicwings.com/arduino/arduino-i2c
#define SCL_PIN 22            //// serial clock pin 
#endif

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
MPU9250_asukiaaa mySensor;
LoRaModem modem(Serial1);
float mDirection, mX, mY, mZ, baseMz, hiMz, loMz, hiRst, loRst; // x,y,z axis magnetometer readings. 
String msg;                                                     // BaseMz sets the first initial z axis reading to the baseline value (zeroing out the scale)
int mZCounter = 0;                                              // hiMz, LoMz hi and low ranges from the baseline value
bool sensor;

// *** fix me *** "String appEui" is actually the device Eui ---> change to devEui

//String appEui = "a8610a34343c800f";//MR2                       
String devEui = "a8610a343133920b"; //nsx
//String appKey = "a8610a343429670f"; // supra
//String appKey = "a8610a3434325d13"; // 400z
String appKey = "7d4c6e125fc7bf69b41814bfbcaa812b";

void sendMessage(bool present) {
  if (present) {
    msg = "true";
  } else {
    msg = "false";
  }
      int err;
      modem.beginPacket();
      modem.print(msg);
      err = modem.endPacket(true);
      if (err > 0) {
        Serial.println("Message sent correctly!");
      } else {
        Serial.println("Error sending message :(");
        Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength"); //// why is this???
        Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");           //// why is this??? This piece came from arduino open source code why does it vary what causes this?? 
      }
      delay(1000);
      if (!modem.available()) {
        Serial.println("No downlink message received at this time.");
        return;
      }
      char rcv[64];
      int i = 0;
      while (modem.available()) {
        rcv[i++] = (char)modem.read();
      }
      Serial.print("Received: ");
      for (unsigned int j = 0; j < i; j++) {
        Serial.print(rcv[j] >> 4, HEX);
        Serial.print(rcv[j] & 0xF, HEX);
        Serial.print(" ");
      }
      Serial.println();
}

void options() {           // can leave for now
  Serial.println();
  Serial.println("Please type an option");
  Serial.println("Type 'sensor' to use the sensor functionality");
  Serial.println("Type 'input' to use the manual true/false input functionality");
  while (!Serial.available());
  String msg = Serial.readStringUntil('\n');

  if (msg == "sensor") {
    sensor = true;
  } else if (msg == "input") {
    sensor = false;
  } else {
    options();
  }
}

void setup() { 
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  // only available US options are US915 and US915-HYBRID
  if (!modem.begin(US915)) {
    Serial.println("Failed to start module");
    while (1) {}
  }
  
  
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(devEui, appKey); 
  while (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    int err;
    while (1) {}
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

  options();
}

void loop() {
      
  if (sensor) {
    VL53L0X_RangingMeasurementData_t measure;
      
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  
    if ((measure.RangeStatus != 4) && (mySensor.magUpdate() == 0)) {  // phase failures have incorrect data
       //For comparison
       loMz = baseMz - 10;
       hiMz = baseMz + 10;
       hiRst = baseMz+100;
       loRst = baseMz-100;
       
       //Magnetomter Z value
       mZ = mySensor.magZ();
       
       //Outputs of ToF, baseMz and observed mZ
      Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter); 
      Serial.println("base: " + String(baseMz));
      Serial.println("magZ: " + String(mZ));

      if (mZ <= loRst || mZ >= hiRst) {
        Serial.println("Going to options");
        options();
      }
  
      //This is just to output a message to know the possible detections
      if ((mZ <= loMz || mZ >= hiMz) && measure.RangeMilliMeter > 90)
      {
        Serial.println("Car is present..");
        sendMessage(true);
      }
      else if ((mZ <= loMz || mZ >= hiMz) && measure.RangeMilliMeter <=90)
      {
        Serial.println("Sense something ferrous, but nothing above..");
        sendMessage(false);
      }
      else if (measure.RangeMilliMeter > 90)
      {
        Serial.println("Car MIGHT be there..");
        sendMessage(false);
      }
      else
      {
        Serial.println("Car not present..");
        sendMessage(false);
      }
      
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
    } else {
      Serial.println(" out of range ");
      Serial.println("Cannot read mag values");
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
      Serial.println();
    }
    delay(10000);
  } else if (!sensor) {
    Serial.println();
    Serial.println("Enter a message to send to network");
    Serial.println("(make sure that end-of-line 'NL' is enabled)");
    Serial.println("Type 'stop' to go back to options");
  
    while (!Serial.available());
    String msg = Serial.readStringUntil('\n');

    if (msg == "stop") {
      options();
    } else {
      Serial.println();
      Serial.print("Sending: " + msg + " - ");
      for (unsigned int i = 0; i < msg.length(); i++) {
        Serial.print(msg[i] >> 4, HEX);
        Serial.print(msg[i] & 0xF, HEX);
        Serial.print(" ");
      }
      Serial.println();
  
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
      delay(15000);
//      if (!modem.available()) {
//        Serial.println("No downlink message received at this time.");
//        return;
//      }
      char rcv[64];
      int i = 0;
      while (modem.available()) {
        rcv[i++] = (char)modem.read();
      }
      Serial.print("Received: ");
      for (unsigned int j = 0; j < i; j++) {
        Serial.print(rcv[j] >> 4, HEX);
        Serial.print(rcv[j] & 0xF, HEX);
        Serial.print(" ");
      }
      Serial.println();
      delay(15000);
      }
  }
}

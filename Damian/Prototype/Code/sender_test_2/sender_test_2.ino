



#include <MKRWAN.h>            
#include <LoRa.h> 

#ifdef _ESP32_HAL_I2C_H_
#endif

LoRaModem modem(Serial);

String msg;                                                     



//String devEui = "a8610a34343c800f";//MR2                       
//String devEui = "a8610a343133920b"; //nsx
//String devEui = "a8610a343429670f"; // supra
String devEui = "a8610a3434325d13"; // 400z
String appKey = "9b5c1ac87326fc646a38c1a64db168e9";
//                 9b5c1ac87326fc646a38c1a64db168e9

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

void setup() {
  LoRa.setFrequency(915E6);
  int TxPower = 20;
  LoRa.setTxPower(TxPower);
  LoRa.setTxPower(TxPower, PA_OUTPUT_PA_BOOST_PIN); // increases transmit power to 20dBm (default is 17dBm). must use "PA_OUTPUT_PA_BOOST_PIN".
  
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  // only available US options are US915
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
    Serial.println("join request failed; move closer to gateway");
    Serial.println("re-sending join request");
    connected = modem.joinOTAA(devEui, appKey);
    while (1) {}
  }

  if (connected = true)
  {
    Serial.println("You are connected");
  }

  // Set poll interval to 60 secs.
}

void loop() {
  Serial.println();
  Serial.println("Enter a message to send to network");
  Serial.println("(make sure that end-of-line 'NL' is enabled)");

  modem.minPollInterval(10);
  // NOTE: independent of this setting, the modem will
  // not allow sending more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
  while (!Serial.available());
  String msg = Serial.readStringUntil('\n');

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
      
 

#include <MKRWAN.h>
#include <LoRa.h>



LoRaModem modem(Serial1);

String appEui = "a8610a34343c800f";//MR2
//String appKey = "4de4012bb6946228c45a7e4c935b95b8"; //nsx
//String appKey = "bd3d5ce14b9b2db692d1829abed7e6b2"; // supra
//String appKey = "35a8eef58147c60bb53666df81ee9ec3"; //mr2
String appKey = "00000000000000000000000000000000";

void sendMessage(bool present) {
  String msg;
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

void setup() {
  int TxPower = 20;
  LoRa.setTxPower(TxPower,PA_OUTPUT_PA_BOOST_PIN); // increases transmit power to 20dBm (default is 17dBm). must use "PA_OUTPUT_PA_BOOST_PIN".  
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
}
void loop(){

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    int err;
    Serial.println("sending packet: hello world");
    modem.beginPacket();
    modem.print("hello world");
    err = modem.endPacket(true);
    while (1) {}
  }
  else
  {
   String msg = "begin";
   while (msg != "done"){
    int err;
    Serial.println("Enter a message: "); 
    msg = Serial.readStringUntil('\n');
    Serial.println("sending packet: " + msg);
    modem.beginPacket();
    modem.print(msg);
    err = modem.endPacket(true);
   }
  }
}

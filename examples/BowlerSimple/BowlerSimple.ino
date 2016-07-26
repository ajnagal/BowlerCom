/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
 int baudrate = 9600;
#include <BowlerCom.h>
int txPin =11;
int rxPin =10;
BowlerCom com;
SoftwareSerial mySerial(rxPin,txPin); // RX, TX

void setup() {

  mySerial.begin(baudrate);
  Serial.begin(baudrate);
  while (!Serial ||!mySerial );    // wait for the serial port to open
  com.begin(&Serial);
  
  startDebugPint(&mySerial);
 
}

void loop() {
  com.server();
}



#include <EEPROM.h>
#include <SoftwareSerial.h>
/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
 int baudrate = 9600;
#include <BowlerCom.h>

BowlerCom com(Serial);
SoftwareSerial mySerial(10,11); // RX, TX



void setup() {
  com.begin(baudrate);
  
  com.addDyIO();
  com.addDyIOPID();
  mySerial.begin(1000000);
  com.startDebugPint(&mySerial);
  setPrintLevelInfoPrint();

}

void loop() {
  com.server();
}


#ifdef ARDUINO_ARCH_ARC32
#include <CurieEEPROM.h>
#endif
#ifdef ARDUINO_ARCH_AVR 
#include <EEPROM.h>
#endif
#include <SoftwareSerial.h>
#include <Servo.h>
/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
 int baudrate = 9600;
#include <BowlerCom.h>

BowlerCom com;
SoftwareSerial mySerial(10,11); // RX, TX

boolean on=false;

void setup() {

  mySerial.begin(baudrate);
  Serial.begin(baudrate);
  while (!Serial ||!mySerial );    // wait for the serial port to open
  com.begin(&Serial);
  startDebugPint(&mySerial);
  
  setPrintLevelInfoPrint();
  
  println_E("Adding namespaces");
  com.addDyIO();  
  //setMode(11,IS_DEBUG_TX);
  setMode(13,IS_DO);
  //com.addDyIOPID();

  println_E("Starting DyIO");
 
}

void loop() {
  com.server();
  SetChanVal(13, on?1:0, 0);
  
  //on=!on;
  //println_I("Bowlewr Server");
}



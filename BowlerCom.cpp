/**
 * Bowler Communication System
 */

#include <BowlerCom.h>

#define comBuffSize 256
byte privateRXCom[comBuffSize];
BYTE_FIFO_STORAGE store;
byte err,i;
BowlerPacket Packet;

float getMs(void){
	return (float)millis();
}
void putCharDebug(char a){
	// none
}
void EnableDebugTerminal(){
	//none
}
BowlerCom::BowlerCom(int baudrate){
	Serial.begin(baudrate);
	InitByteFifo(&store,privateRXCom,comBuffSize);
}
 

void BowlerCom::server(){
	serialEvent();
	if(GetBowlerPacket(&Packet,&store)){
		//Now the Packet struct contains the parsed packet data
		Process_Self_Packet(&Packet);
		// The call backs for processing the packet have been called
		// and the Packet struct now contains the data
		// to be sent back to the client as a response.
		for(i=0;i< GetPacketLegnth(&Packet);i++){
			//Grab the response packet one byte at a time and push it out the physical layer
			Serial.print((char)Packet.stream[i]);
		}
		Serial.flush();
	}
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void BowlerCom::serialEvent() {
  while (Serial.available()) {
    FifoAddByte(&store, (char)Serial.read(), &err);
  }
}

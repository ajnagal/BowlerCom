/**
 * Bowler Communication System
 */

#include <BowlerCom.h>

#define comBuffSize 256
byte privateRXCom[comBuffSize];
BYTE_FIFO_STORAGE store;

BowlerPacket Packet;
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:


float getMs(void) {
	return (float) millis();
}
void putCharDebug(char a) {
	// none
}
void EnableDebugTerminal() {
	//none
}
BowlerCom::BowlerCom(int baudrate) {
	Serial.begin(baudrate);
	while (!Serial) {
	    ; // wait for serial port to connect. Needed for Leonardo only
	  }
	InitByteFifo(&store, privateRXCom, comBuffSize);
}

void BowlerCom::server() {
	while (Serial.available()) {
		// get the new byte:
		char inChar = (char) Serial.read();
		addByte(inChar);
	}
	if (GetBowlerPacket(&Packet, &store)) {
		//Now the Packet struct contains the parsed packet data
		Process_Self_Packet(&Packet);
		// The call backs for processing the packet have been called
		// and the Packet struct now contains the data
		// to be sent back to the client as a response.
		int i;
		for (i = 0; i < GetPacketLegnth(&Packet); i++) {
			//Grab the response packet one byte at a time and push it out the physical layer
			Serial.write((char) Packet.stream[i]);
		}
	}
}
void BowlerCom::addByte(byte b){
	byte err;
	FifoAddByte(&store, b, &err);

}


/**
 * Bowler Communication System
 */

#include <BowlerCom.h>


static byte privateRXCom[comBuffSize];
static BYTE_FIFO_STORAGE store;

static BowlerPacket Packet;
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

	pinMode(11, OUTPUT);
	digitalWrite(11, LOW );
	pinMode(12, OUTPUT);
	digitalWrite(12, LOW );
}

void BowlerCom::server() {
	while (Serial.available()>0) {
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
		for (i = 0; i < (int)GetPacketLegnth(&Packet); i++) {
			//Grab the response packet one byte at a time and push it out the physical layer
			Serial.write((char) Packet.stream[i]);

		}
		digitalWrite(11, HIGH );
		digitalWrite(12, LOW );
	}
}
void BowlerCom::addByte(byte b){
	byte err;
	FifoAddByte(&store, b, &err);
	digitalWrite(12, HIGH );
	digitalWrite(11, LOW );
}


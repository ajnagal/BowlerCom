/**
 * Bowler Communication System
 */
#include <Arduino.h>
#include <BowlerCom.h>


float getMs(void) {
	return (float) millis();
}
void putCharDebug(char a) {
	// none
}
void EnableDebugTerminal() {
	//none
}
BowlerCom::BowlerCom(Stream &s) : BowlerSerial(s)
{

}

/* begin method for overriding default serial bitrate */
void BowlerCom::begin(void)
{
  begin(9600);
}

/* begin method for overriding default serial bitrate */
void BowlerCom::begin(long speed)
{
  Serial.begin(speed);
  begin(Serial);
}

void BowlerCom::begin(Stream &s)
{
	BowlerSerial = s;
	InitByteFifo(&store, privateRXCom, comBuffSize);

}
boolean BowlerCom::PutBowlerPacketLocal(BowlerPacket * p) {
	int i;
	for (i = 0; i < (int)GetPacketLegnth(p); i++) {
		//Grab the response packet one byte at a time and push it out the physical layer
		BowlerSerial.write((char) p->stream[i]);
	}
    return true;
}
void BowlerCom::server(void) {
	byte err;
	while (Serial.available()>0) {


		FifoAddByte(&store, (char) BowlerSerial.read(), &err);
	}
	if (GetBowlerPacket(&Packet, &store)) {
		//Now the Packet struct contains the parsed packet data
		Process_Self_Packet(&Packet);
		// The call backs for processing the packet have been called
		// and the Packet struct now contains the data
		// to be sent back to the client as a response.

	}
	RunNamespaceAsync(&Packet, &PutBowlerPacketLocal);
}


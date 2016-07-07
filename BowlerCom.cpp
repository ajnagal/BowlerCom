/**
 * Bowler Communication System
 */
#include <Arduino.h>
#include <BowlerCom.h>

static BowlerCom * ref;
static SoftwareSerial * serialPort = NULL;

float getMs(void) {
	return ((float) micros()) * 1000.0f;
}
void putCharDebug(char a) {
	// none
	if(serialPort!=NULL){
		serialPort->print(a);
	}

}
void EnableDebugTerminal() {
	//none
}
BowlerCom::BowlerCom(Stream &s) :
		BowlerSerial(s) {
	ref = this;

}

/* begin method for overriding default serial bitrate */
void BowlerCom::begin(void) {
	begin(9600);
}

/* begin method for overriding default serial bitrate */
void BowlerCom::begin(long speed) {
	Serial.begin(speed);
	begin(Serial);
}

void BowlerCom::begin(Stream &s) {
	BowlerSerial = s;
	InitByteFifo(&store, privateRXCom, comBuffSize);

}
uint16_t putStream(uint8_t * buffer, uint16_t datalength) {
	uint16_t i;
	for (i = 0; i < datalength; i++) {
		//Grab the response packet one byte at a time and push it out the physical layer
		ref->BowlerSerial.write((char) buffer[i]);
	}
	return true;
}
void BowlerCom::server(void) {
	byte err;
	byte newByte = 0;
	while (Serial.available() > 0) {
		newByte = BowlerSerial.read();
		println_I("Adding byte: ");prHEX8(newByte,INFO_PRINT);
		FifoAddByte(&store, (char) newByte, &err);
	}
	if (GetBowlerPacket(&Packet, &store)) {
		//Now the Packet struct contains the parsed packet data
		Process_Self_Packet(&Packet);
		// The call backs for processing the packet have been called
		// and the Packet struct now contains the data
		// to be sent back to the client as a response.
		PutBowlerPacket(&Packet);
	}
	RunNamespaceAsync(&Packet, &PutBowlerPacket);
}
void BowlerCom::addDyIO() {
	InitPinFunction();

	addNamespaceToList(get_bcsIoNamespace());
	//println_I("Adding IO.Setmode Namespace");
	addNamespaceToList(get_bcsIoSetmodeNamespace());
	//println_I("Adding DyIO Namespace");
	addNamespaceToList(get_neuronRoboticsDyIONamespace());
	//println_I("Adding PID Namespace");
	addNamespaceToList(getBcsPidNamespace());
	//println_I("Adding DyIO PID Namespace");
	addNamespaceToList(get_bcsPidDypidNamespace());
	//println_I("Adding Safe Namespace");
	addNamespaceToList( get_bcsSafeNamespace());

}
void BowlerCom::startDebugPint(SoftwareSerial * port){
	if(port == NULL)
		return;
	serialPort = port;
	setPrintLevelInfoPrint();
	println_E("Welcome To Arduino DyIO!");
}


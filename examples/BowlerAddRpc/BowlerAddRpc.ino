/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
int baudrate = 9600;
#include <BowlerCom.h>
int txPin = 11;
int rxPin = 10;
BowlerCom com;
SoftwareSerial mySerial(rxPin, txPin); // RX, TX

#define comBuffSize 256
static BYTE privateRXCom[comBuffSize];
static BYTE_FIFO_STORAGE store;
static BYTE err, i;
static BowlerPacket Packet;
boolean processTest(BowlerPacket * Packet) {
	// process packet here
	uint8_t i = 0;
	float divisor = ((float) get32bit(Packet, 0)) / 1000.0f; // pull out a fixed point into a float
	int size = Packet->use.data[4]; //size of the list of numbers

	for (i = 0; i < size; i++) {
		int value = get32bit(Packet, 5 + (i * 4)); // extract the value int he list of valieus
		float calculation = ((float) value) / divisor;
		set32bit(Packet, // the packet struct referenc to write to
				(int) (calculation * 1000.0f), // the value scaled up for transport
				3 + (i * 4)); // calculate the offset in the packet
	}
	Packet->use.data[0] = 37; // set the int
	Packet->use.data[1] = 1; // set the boolean as 1 or 0
	Packet->use.data[2] = size; // set the size of the array of floats
}
NAMESPACE_LIST testns = { "test.ns.*;1.0;;", // The string defining the namespace
		NULL, // the first element in the RPC list
		NULL, // async for this namespace, no async yet
		NULL // no initial elements to the other namesapce field.
		};
RPC_LIST test_ns_test_get = { BOWLER_GET, "test", &processTest,
// see c-bowler/BowlerStack/include/Bowler/Defines.h for all datatypes
		{ BOWLER_FIXED1K, // fixed point value scaled by 1000 to an integer
				BOWLER_I32STR, // a list of numbers as 32 bit signed integers
				0 }, // receive arguments
		BOWLER_POST, // response method
		{ BOWLER_I08, //8 bit ints
				BOWLER_BOOL, // boolean values
				BOWLER_FIXED1K_STR, 0 }, // Response arguments
		NULL //next list element, null when declared
		};

void setup() {

	mySerial.begin(baudrate);
	Serial.begin(baudrate);
	while (!Serial || !mySerial)
		;    // wait for the serial port to open
	com.begin(&Serial);

	startDebugPint(&mySerial);

	  setPrintLevelErrorPrint();
	// add the new RPC to its namespace
	addRpcToNamespace(&testns, &test_ns_test_get);
	//add namespace to stack
	addNamespaceToList(&testns);

}

void loop() {
	com.server();
}


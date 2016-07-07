/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
#define comBuffSize 256
#include "BowlerStack/include/Bowler/Bowler.h"
#include "BowlerStack/include/DyIO/PID.h"
#include "BowlerStack/include/DyIO/DyIO_def.h"
#include "BowlerStack/include/Namespace/AsyncManager.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io_setmode.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io.h"
/* Borrwoing heavily from Boards.h - Hardware Abstraction Layer for Firmata library */

class BowlerCom {
public:
	BowlerCom(Stream &s);
	void server(void);
	/* Arduino constructors */
	void begin();
	void begin(long);
	void begin(Stream &s);
	boolean PutBowlerPacketLocal(BowlerPacket * p);
	void addDyIO();
	void startDebugPint(int tx, int rx, int baud);
	Stream &BowlerSerial;
	byte privateRXCom[comBuffSize];
	BYTE_FIFO_STORAGE store;
	BowlerPacket Packet;

};

#endif

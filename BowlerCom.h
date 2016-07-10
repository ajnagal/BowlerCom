/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
#define comBuffSize 256
#include <Arduino.h>
#ifdef __cplusplus
	#include <EEPROM.h>
	#include <SoftwareSerial.h>
	#include <Servo.h>
#endif

#include "BowlerStack/include/Bowler/Bowler.h"
#include "BowlerStack/include/DyIO/PID.h"
#include "BowlerStack/include/DyIO/DyIO_def.h"
#include "BowlerStack/include/Namespace/AsyncManager.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io_setmode.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io.h"


#ifdef __cplusplus
/**
 * Start the serial debugger
 */
void startDebugPint(SoftwareSerial * port);
/**
 * BowlerCom Class
 * This class is for handeling the Bowler Communications on a stream.
 */
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
		void addDyIOPID();

		Stream &BowlerSerial;
		byte privateRXCom[comBuffSize];
		BYTE_FIFO_STORAGE store;
		BowlerPacket Packet;
	private:
		boolean addedDyIO;
		boolean addedPID;
	};
#endif
#endif

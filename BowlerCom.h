/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
#define comBuffSize 256
#include <Arduino.h>
#ifdef __cplusplus
	#ifdef ARDUINO_ARCH_ARC32
#if !defined(EEPROM_SIZE)
	#include <CurieEEPROM.h>
#endif
	#endif
	#ifdef ARDUINO_ARCH_AVR
	#include <EEPROM.h>
	#define EEPROM_SIZE 1024 //EEPROM size in dwords
	#endif
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
		BowlerCom();
		void server(void);
		/* Arduino constructors */
		void begin();
		void begin(long);
		boolean PutBowlerPacketLocal(BowlerPacket * p);
		void addDyIO();
		void addDyIOPID();

		byte privateRXCom[comBuffSize];
		BYTE_FIFO_STORAGE store;
		BowlerPacket Packet;
	private:
		boolean addedDyIO;
		boolean addedPID;
		void begin_local();
	};
#endif
#endif

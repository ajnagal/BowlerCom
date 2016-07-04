/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
 #define comBuffSize 256
#include "BowlerStack/include/Bowler/Bowler.h"
 
class BowlerCom {
public:
	BowlerCom(Stream &s);
	void server(void);
	/* Arduino constructors */
	    void begin();
	    void begin(long);
	    void begin(Stream &s);
private:
	    Stream &BowlerSerial;
	byte privateRXCom[comBuffSize];
	BYTE_FIFO_STORAGE store;
	BowlerPacket Packet;

};
 
#endif

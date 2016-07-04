/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
 #define comBuffSize 256
#include "BowlerStack/include/Bowler/Bowler.h"
 
class BowlerCom {
public:
	BowlerCom(int baudrate);
	void server();
	void addByte(byte b);
};
 
#endif

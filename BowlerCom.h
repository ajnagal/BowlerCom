/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
 
#include "BowlerStack/include/Bowler/Bowler.h"
 
class BowlerCom {
public:
	BowlerCom(int baudrate);
	void server();
private:
	void serialEvent();
};
 
#endif

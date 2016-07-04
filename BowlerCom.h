/**
 * Bowler Communication System
 */

#ifndef ARDUBCS_H
#define ARDUBCS_H
 
#include <Arduino.h>
#include "BowlerStack/include/Bowler/Bowler.h"
 
class BowlerCom {
public:
	BowlerCom();
	void initBowler();
};
 
#endif

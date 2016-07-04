#ifndef BOWLERCONFIG_H_
#define BOWLERCONFIG_H_


#include <Arduino.h>
#define StartCritical() noInterrupts()
#define EndCritical()   interrupts()

#define getMs() millis()

#define putCharDebug(a) nop()

#endif

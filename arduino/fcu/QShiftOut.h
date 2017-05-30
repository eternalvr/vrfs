// QShiftOut.h

#ifndef _QSHIFTOUT_h
#define _QSHIFTOUT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class QShiftOut
{
 protected:
	 uint8_t dtimer;
	 uint8_t dbit;
	 uint8_t dport;
	 uint8_t ctimer;
	 uint8_t cbit;
	 uint8_t cport;
	 volatile uint8_t *dout;
	 volatile uint8_t *cout;
	 uint8_t i;
	 uint8_t oldSREG;
	 uint8_t bitOrder;

 public:
	void init(const uint8_t datapin, const uint8_t clockpin, const uint8_t bitOrder);
	void shiftOut(uint8_t val);
	void turnOffPWM(uint8_t timer);
};


#endif


// 
// 
// 

#include "QShiftOut.h"

void QShiftOut::init(const uint8_t datapin, const uint8_t clockpin, const uint8_t bitorder)
{

	this->dtimer = digitalPinToTimer(datapin);
	this->dbit = digitalPinToBitMask(datapin);
	this->dport = digitalPinToPort(datapin);
	
	this->ctimer = digitalPinToTimer(clockpin);
	this->cbit = digitalPinToBitMask(clockpin);
	this->cport = digitalPinToPort(clockpin);
	

	if (dport == NOT_A_PIN) return;
	if (cport == NOT_A_PIN) return;

	// If the pin that support PWM output, we need to turn it off
	// before doing a digital write.
	if (dtimer != NOT_ON_TIMER) turnOffPWM(dtimer);
	if (ctimer != NOT_ON_TIMER) turnOffPWM(ctimer);
	dout = portOutputRegister(dport);
	cout = portOutputRegister(cport);
	
	
	bitOrder = bitorder;
}
void QShiftOut::shiftOut(uint8_t val)
{
	
	for (i = 0; i < 8; i++) {
		if (bitOrder == LSBFIRST) {
			if (!!(val & (1 << i)) == LOW) {
				*dout &= ~dbit;
			}
			else {
				*dout |= dbit;
			}
		}
		else {
			if ((!!(val & (1 << (7 - i)))) == LOW) {
				*dout &= ~dbit;
			}
			else {
				*dout |= dbit;
			}
		}
		// Write HIGH
		*cout |= cbit;
		// Write LOW
		*cout &= ~cbit;
		
	}
	
}
void QShiftOut::turnOffPWM(uint8_t timer)
{
	switch (timer)
	{
	case TIMER1A:   cbi(TCCR1A, COM1A1);    break;
	case TIMER1B:   cbi(TCCR1A, COM1B1);    break;
	case  TIMER0A:  cbi(TCCR0A, COM0A1);    break;
	case  TIMER0B:  cbi(TCCR0A, COM0B1);    break;
	case  TIMER2A:  cbi(TCCR2A, COM2A1);    break;
	case  TIMER2B:  cbi(TCCR2A, COM2B1);    break;
	}
}


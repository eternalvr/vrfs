// FCUDisplay.h

#ifndef _FCUDISPLAY_h
#define _FCUDISPLAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <WString.h>

class FCUDisplay
{
 protected:
	 static void UpdateDisplayString();

	 
	
 public:
	 static String DisplayString;
	 enum DISPLAYTYPE { SPEED, HEADING, ALTITUDE, VSPEED };
	 static int Speed;
	 static int Heading;
	 static int Altitude;
	 static int VSpeed;
	 
	 static void SetMessage(String message);
	 static bool Dirty;

	 
	 void SetDisplayValue(DISPLAYTYPE dt, int value, bool immediate_update);

	 void SetDisplayMessage(String message);

};



#endif


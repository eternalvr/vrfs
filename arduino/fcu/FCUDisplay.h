/** A320 FCU Arduino 
Copyright (C) 2017  Volker Richter <v[a]vrmx.eu>

This file is part of FCU.

FCU is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

FCU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FCU.  If not, see <http://www.gnu.org/licenses/>.
*/
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


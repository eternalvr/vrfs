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
// FCU.h

#ifndef _FCU_h
#define _FCU_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "fcu_pins.h"
#include "fcu_limits.h"
#include <Encoder.h>
class FCU
{
 private:
	 char cBuf3[3];
	 char cBuf5[5];
	 
	 long oldVal = 0;

 protected:
	 
	 void setupPins();

	 void setStartModes();
	 
	 

 public:
	enum Mode { Managed, Selected };
	enum DisplayModeSpeed { KTS, MACH };
	enum DisplayModeVS {VS, FPA};
	enum DISPLAYTYPE { SPEED, HEADING, ALTITUDE, VSPEED };

	Mode SPDMode = Selected;
	Mode HDGMode = Selected;
	Mode ALTMode = Selected;

	DisplayModeSpeed SpeedMode = KTS;
	DisplayModeVS VSMode = VS;

	signed int VerticalSpeed  = 0;
	int Speed = 0;
	int SpeedMach = 0;
	int fpa = 30;

	long Altitude = 0;
	int Heading = 0;
	bool VSDashed = false;

	bool displayDirty = false;
	

	void Initialize(int speed, int hdg, int alt, int vs);
	void SetSpeedMode(DisplayModeSpeed sm);
	void SetVSMode(DisplayModeVS vs);

	void UpdateDisplayString();

	void SetDisplayValue(DISPLAYTYPE dt, long value, bool immediate_update);

	void SetDisplayMessage(String message);

	void SetEncoderValue(DISPLAYTYPE type, int32_t value, Encoder * enc);

	String DisplayString;
};


#endif


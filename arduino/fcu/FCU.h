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


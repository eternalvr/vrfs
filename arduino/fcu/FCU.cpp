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

#include "FCU.h"





void FCU::Initialize(int speed, int hdg,  int alt, int vs)
{
	setupPins();
	setStartModes();
	VSDashed = true;
	VerticalSpeed = -998;
}

void FCU::setupPins()
{
	int pins[] = { LED_EXPED, LED_AP2, LED_APPR, LED_LOC, LED_ATHR, LED_AP1, LED_MODE_MACH, LED_MODE_SPEED, LED_MODE_HDG, LED_MODE_TRK, LED_MANAGED_ALT, LED_MANAGED_HDG, LED_MANAGED_SPD };
	for (int i = 0; i < sizeof(pins)*sizeof(int); i++)
	{
		pinMode(pins[i], OUTPUT);
	}
}

void FCU::setStartModes()
{
	
	SetSpeedMode(DisplayModeSpeed::KTS);
	SetVSMode(DisplayModeVS::VS);
	displayDirty = true;
	UpdateDisplayString();
}

void FCU::SetSpeedMode(DisplayModeSpeed sm)
{
	digitalWrite(LED_MODE_SPEED, 0);
	digitalWrite(LED_MODE_MACH, 0);

	digitalWrite((sm == DisplayModeSpeed::KTS) ? LED_MODE_SPEED : LED_MODE_MACH, 1);
}
void FCU::SetVSMode(DisplayModeVS vs)
{
	digitalWrite(LED_MODE_HDG, 0);
	digitalWrite(LED_MODE_TRK, 0);

	digitalWrite((vs == DisplayModeVS::VS) ? LED_MODE_HDG : LED_MODE_TRK, 1);
}
void FCU::UpdateDisplayString()
{

	if (displayDirty == false) {
		
		return;
	}
	String tmpBuf = "";
	
	// SPEED
	if (SPDMode == Mode::Managed) {
		digitalWrite(LED_MANAGED_SPD, 1);
		tmpBuf.concat("---");
	}
	else {
		digitalWrite(LED_MANAGED_SPD, 0);
		
		if (SpeedMode == DisplayModeSpeed::KTS) {
			if (Speed == -998) {
				sprintf(cBuf3, "   ", Speed);
			} else if(Speed == -999) {
				sprintf(cBuf3, "---", Speed);
				
			}
			else {
				sprintf(cBuf3, "%03i", Speed);
			}
			
		}
		else {
			sprintf(cBuf3, "0%2i", SpeedMach);		
		}
		tmpBuf.concat(cBuf3);
	}

	// HDG
	if (HDGMode == Mode::Managed) {
		digitalWrite(LED_MANAGED_HDG, 1);
		tmpBuf.concat("---");
	}
	else {
		digitalWrite(LED_MANAGED_HDG, 0);
		if (Heading == -998) {
			sprintf(cBuf3, "   ", Heading);
		}
		else if (Heading == -999) {
			sprintf(cBuf3, "---", Heading);

		}
		else {
			sprintf(cBuf3, "%03i", Heading);
		}
		tmpBuf.concat(cBuf3);
	}

	//ALT
	if (ALTMode == Mode::Managed) {
		digitalWrite(LED_MANAGED_ALT, 1);
		sprintf(cBuf5, "%05li", Altitude);
		tmpBuf.concat(cBuf5);
	}
	else {
		digitalWrite(LED_MANAGED_ALT, 0);
		if (Altitude == -998) {
			sprintf(cBuf5, "     ", Altitude);
		}
		
		else {
			sprintf(cBuf5, "%05li", Altitude);
		}
		tmpBuf.concat(cBuf5);
	}

	if (VSMode == DisplayModeVS::VS) {
		

		if (VSDashed) {
			tmpBuf.concat("-----");
		}
		else {
			if (VerticalSpeed == -998) {
				sprintf(cBuf5, "     ", VerticalSpeed);
			}
			else if (VerticalSpeed == -9999) {
				VSDashed = true;

				sprintf(cBuf5, "-----", VerticalSpeed);

			}
			else {
				
				VSDashed = false;
				sprintf(cBuf5, "%5i", VerticalSpeed);
				if (VerticalSpeed != 0) {
					cBuf5[3] = 'o';
					cBuf5[4] = 'o';
				}
				
			}

			
			tmpBuf.concat(cBuf5);
		}
	}
	else {
		char t[2];
		sprintf(t, "%2i", fpa);
		tmpBuf.concat(t);
		tmpBuf.concat("  ");
	}
	


	DisplayString = tmpBuf;
	
	displayDirty = false;
}
void FCU::SetDisplayValue(DISPLAYTYPE dt, long value, bool immediate_update = true)
{
	value = value / 4;
	switch (dt) {
	case DISPLAYTYPE::ALTITUDE:
		Altitude = value; break;
	case DISPLAYTYPE::HEADING:
		Heading = value; break;
	case DISPLAYTYPE::SPEED:
		if (Speed < 100) Speed = 100;
		if (Speed > 350) Speed = 350;
		Speed = value; break;
	case DISPLAYTYPE::VSPEED:
		VerticalSpeed = value; break;
	}

	displayDirty = true;

	if (immediate_update) {
		UpdateDisplayString();
	}

}
void FCU::SetDisplayMessage(String message)
{

	if (message.length() > 16) {
		message = message.substring(0, 16);
	}
	if (message.length() < 16) {
		String tmpString = "                ";
		for (int i = 0; i < message.length(); i++)
		{
			tmpString[i] = message[i];
		}
		DisplayString = tmpString;
		return;
	}

	DisplayString = message;
}
void FCU::SetEncoderValue(DISPLAYTYPE type, int32_t value, Encoder *enc)
{
	
	value = value / ENC_PULSE;
	switch (type) {
	case DISPLAYTYPE::ALTITUDE:
		
		oldVal = Altitude;
		Altitude = value*1000;
		
		if (Altitude >= ALT_LIMIT_MAX) {
			Altitude = ALT_LIMIT_MAX;
			enc->write(ALT_LIMIT_MAX/1000 * ENC_PULSE);
		} else if (Altitude < ALT_LIMIT_MIN) {
			Altitude = ALT_LIMIT_MIN;
			enc->write(ALT_LIMIT_MIN/1000 * ENC_PULSE);
		}
		if (oldVal != Altitude) { displayDirty = true; }
		break;
	case DISPLAYTYPE::HEADING:
		if (Heading < -997 || value == 5000) {
			enc->write(5000);
		}
		else {
			oldVal = Heading;
			Heading = value;
			if (Heading > HDG_LIMIT_MAX) {
				Heading = HDG_LIMIT_MIN;
				enc->write(HDG_LIMIT_MIN * ENC_PULSE);
			}
			else if (Heading < HDG_LIMIT_MIN) {
				Heading = HDG_LIMIT_MAX;
				enc->write(HDG_LIMIT_MAX * ENC_PULSE);
			}
		}
		if (oldVal != Heading) { displayDirty = true; }
		break;
	case DISPLAYTYPE::SPEED:
		if (Speed < -997 || value == 5000) {
			enc->write(5000);
		}

		
		else {
			oldVal = Speed;
			Speed = value;
			if (Speed >= SPD_LIMIT_MAX) {
				Speed = SPD_LIMIT_MAX;
				enc->write(SPD_LIMIT_MAX * ENC_PULSE);
			}
			else if (Speed < SPD_LIMIT_MIN) {
				Speed = SPD_LIMIT_MIN;
				enc->write(SPD_LIMIT_MIN * ENC_PULSE);
			}
			
		}
		if (oldVal != Speed) { displayDirty = true; }
		break;
	case DISPLAYTYPE::VSPEED:
		Serial.print("SEV: Vert: ");
		Serial.print(VerticalSpeed);
		Serial.print(" value: ");
		Serial.println(value);
		if (VerticalSpeed < -997 || value == 5000) {
			enc->write(5000);
		}
		else {

			

			oldVal = VerticalSpeed;
			VerticalSpeed = value * 100;
			if (VerticalSpeed >= VS_LIMIT_MAX) {
				VerticalSpeed = VS_LIMIT_MAX;
				enc->write(VS_LIMIT_MAX * ENC_PULSE);
			}
			if (VerticalSpeed <= VS_LIMIT_MIN) {
				Serial.println("Setting VS to min");
				VerticalSpeed = VS_LIMIT_MIN;
				enc->write(VS_LIMIT_MIN * ENC_PULSE);
			}
		}
		if (oldVal != VerticalSpeed) { displayDirty = true; }
	}
	
	
}
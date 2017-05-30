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
#include "FCUDisplay.h"
String FCUDisplay::DisplayString = "";
int FCUDisplay::Speed = 0;
int FCUDisplay::Heading = 0;
int FCUDisplay::Altitude = 0;
int FCUDisplay::VSpeed = 0;
bool FCUDisplay::Dirty = false;

void FCUDisplay::UpdateDisplayString()
{
	char tmpBuf[16];
	sprintf(tmpBuf, "%3i%3i%5i%5i", Speed, Heading, Altitude, VSpeed);
	
	DisplayString = tmpBuf;
	FCUDisplay::Dirty = false;
}





void FCUDisplay::SetMessage(String message)
{
  
	if (message.length() > 16) {
		message = message.substring(0, 16);  
	}
	if (message.length() < 16) {
    String tmpString = "                ";
		for(int i = 0;i < message.length();i++)
		{
		  tmpString[i] = message[i];
		}
   DisplayString = tmpString;
   return;
	}
 
	DisplayString = message;
}



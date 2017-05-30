

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



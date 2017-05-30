A320 FCU Arduino 
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

#include <SimpleTimer.h>
#include <Button.h>
#include <UIPEthernet.h>
#include "fcu_pins.h"
#include "fcu_limits.h"
#include "FCU.h"
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include "FCUDisplay.h"
#include "FCUDisplay.h"
#include "QShiftOut.h"
#include "FastOut.h"
#include <config.h>
#define USE_HARD_SPI 0
#include <TPIC6B595.h>
#include "FastShiftOut.h"
#include <SPI.h>
#include "LedControl.h"
#include <net.h>
#include "StopWatch.h"
#include "QShiftOut.h"
#include <FrequencyTimer2.h>
#include <EEPROM.h>


// DEFINES

#define USE_NETWORK 1

#define SERVER_PORT 1666
#define LOCAL_PORT 1667

#define LOCK_TIME 200
#define PAUSE_AFTER_BUTTON_PUSH 2000
#define SERVER_TIMEOUT 5000

#define REQUEST_RATE 5000 // milliseconds

// GLOBALS

LedControl lc = LedControl(MAX_DATA_IN, MAX_CLK, MAX_LOAD, 1);
QShiftOut qs;
Encoder encSPD = Encoder(ENC_E4A, ENC_E4B);
Encoder encHDG = Encoder(ENC_E3A, ENC_E3B);
Encoder encALT = Encoder(ENC_E2A, ENC_E2B);
Encoder encVS = Encoder(ENC_E1A, ENC_E1B);
FCU fcu;
IPAddress serverIP = IPAddress(192, 168, 5, 67);
EthernetUDP udp;
// ButtonList
uint8_t ButtonList[] = { ENC_E1PS, ENC_E1PL, ENC_E2PS, ENC_E2PL, ENC_E3PL, ENC_E3PS, ENC_E4PL, ENC_E4PS, SW_AP1, SW_AP2, SW_APPR, SW_ATHR, SW_EXPED, SW_HDGTRK, SW_LOC, SW_METALT, SW_SPDMACH };
SimpleTimer networkTimer = SimpleTimer();

Button *PushButtons[sizeof(ButtonList)];
byte PushButtonCounter = 0;
int l = 1;
int activeDevice = 0;

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };


static long timer;
int idx = 0;
uint8_t tpic_prt;
uint8_t tpic_rck_bit;
volatile uint8_t *tpic_rck_port;
int lastTime = 0;

byte encoderADetent = 0;
unsigned char udpPacketBuffer[32];
uint32_t encSPDValue, encHDGValue, encALTValue, encVSValue;
uint32_t encLastSPDValue, encLastHDGValue, encLastALTValue, encLastVSValue;

unsigned long pushTime = 0;
unsigned long dontSendTime = 0;

int lastDottedSpeed = 0;
int lastDottedHeading = 0;
int lastDottedAlt = 0;

bool shouldSendStatus = false;



void setup() {

	Serial.begin(57600);
	

	initializeSwitches();
	initializeButtons();

	pinMode(TPIC_SERIAL_IN, OUTPUT);
	pinMode(TPIC_SRCK, OUTPUT);
	pinMode(TPIC_RCK, OUTPUT);
	pinMode(TPIC_CLR, OUTPUT);
	digitalWrite(TPIC_CLR, HIGH);

	qs.init(TPIC_SERIAL_IN, TPIC_SRCK, MSBFIRST);

	fcu.Initialize(150,150,5000,-998);
	encSPD.write(150 * ENC_PULSE);
	encHDG.write(150 * ENC_PULSE);
	encALT.write(5 * ENC_PULSE);
	encVS.write(5000);


	FrequencyTimer2::setPeriod(64000);
	FrequencyTimer2::setOnOverflow(switchDevices);
	FrequencyTimer2::enable();


	tpic_prt = digitalPinToPort(TPIC_RCK);
	tpic_rck_bit = digitalPinToBitMask(TPIC_RCK);
	tpic_rck_port = portOutputRegister(tpic_prt);

	for (int i = 0; i < lc.getDeviceCount(); i++) {
		lc.shutdown(i, false);
		lc.setIntensity(i, MAX_INTENSITY);
		lc.clearDisplay(i);
		lc.setScanLimit(i, 0);
	}


	ledTest();

#if USE_NETWORK
	initializeNetwork();
#endif

}
void ledTest()
{
	fcu.SetDisplayMessage("8888888888888888");
	byte pins[] = { LED_EXPED, LED_AP2, LED_APPR, LED_LOC, LED_ATHR, LED_AP1, LED_MODE_MACH, LED_MODE_SPEED, LED_MODE_HDG, LED_MODE_TRK, LED_MANAGED_ALT, LED_MANAGED_HDG, LED_MANAGED_SPD };
	for (int i = 0; i < sizeof(pins); i++) {
		digitalWrite(pins[i], 1);
	}
	delay(1000);
	for (int i = 0; i < sizeof(pins); i++) {
		digitalWrite(pins[i], 0);
	}
	delay(1000);
}


void initializeButtons()
{
	for (byte i = 0; i < sizeof(ButtonList)*sizeof(uint8_t); i++)
	{
		PushButtons[i] = new Button(ButtonList[i], BUTTON_PULLUP_INTERNAL, true, 50);
		PushButtons[i]->pressHandler(onPushButton);
		PushButtons[i]->releaseHandler(onPushButton);
	}
}

void onPushButton(Button &b)
{

	if (PushButtons[15]->isPressed(false) && PushButtons[16]->isPressed(false)) {
		Serial.println("SoftReset!");
		softReset();
	}

	sendButtonPacket();
}


void initializeNetwork()
{
	fcu.SetDisplayMessage("DHCP");
	if (Ethernet.begin(mymac)) {
		fcu.SetDisplayMessage("IP");
		char buf[6], buf2[6];
		sprintf(buf, "%5i", Ethernet.localIP()[2]);
		sprintf(buf2, "%5i", Ethernet.localIP()[3]);
	
		fcu.SetDisplayMessage(String(Ethernet.localIP()[0], 10) + String(Ethernet.localIP()[1], 10) + String(buf) + String(buf2));
		delay(2000);
		byte serverLastOctet = EEPROM.read(0);
		byte serverLastValue = 0;

		unsigned long timeout = millis();
		char dispBuf[3];
		
		encVS.write(serverLastOctet*ENC_PULSE);
		
		while (digitalRead(ENC_E1PS) && millis() < timeout + SERVER_TIMEOUT) {
			
			sprintf(dispBuf, "%03i", serverLastOctet);
			fcu.SetDisplayMessage("SERVERIP     " + String(dispBuf));
			serverLastOctet = encVS.read() / 4;
			
			if (serverLastValue != serverLastOctet) {
				timeout = millis();
				serverLastValue = serverLastOctet;
			}
		}

		EEPROM.write(0, serverLastOctet);

		serverIP = IPAddress(Ethernet.localIP()[0], Ethernet.localIP()[1], Ethernet.localIP()[2], (uint8_t)serverLastOctet);


		networkTimer.setInterval(250, sendStatusPacket);
		udp.begin(LOCAL_PORT);
		Serial.println("UDP BEGIN");
	}
	else {
		fcu.SetDisplayMessage("DHCP fail");
		delay(10000);
		return;
	}

	sendStatusPacket();
}


void initializeSwitches()
{
	for (int i = A8; i <= A15; i++)
	{
		pinMode(i, INPUT_PULLUP);
	}
}
void switchOldDevices()
{
	/*digitalWrite(TPIC_RCK, LOW);
	shiftOut(TPIC_SERIAL_IN, TPIC_SRCK, MSBFIRST, 0);
	shiftOut(TPIC_SERIAL_IN, TPIC_SRCK, MSBFIRST, 0);
	digitalWrite(TPIC_RCK, HIGH);*/
}
void switchDevices()
{
	// prevent race conditions between updates
	if (fcu.DisplayString.length() != 16) {
		return;
	}

	 lc.shutdown(0, false);
   for(idx = 0; idx < 8; idx++)
   {

	  *tpic_rck_port &= ~tpic_rck_bit; // LOW 
	   qs.shiftOut(0); qs.shiftOut(0);
	  *tpic_rck_port |= tpic_rck_bit; // HIGH

	  lc.setChar(0, 0, fcu.DisplayString.charAt(idx+8), false);         
	  
	  delayMicroseconds(120);
	  
	  *tpic_rck_port &= ~tpic_rck_bit; // LOW
	  qs.shiftOut(1 << idx);
	  qs.shiftOut(0);
	  *tpic_rck_port |= tpic_rck_bit; // HIGH
   }  
   for (idx = 0; idx < 8; idx++)
   {
	   *tpic_rck_port &= ~tpic_rck_bit; // LOW 
	   qs.shiftOut(0); qs.shiftOut(0);
	   *tpic_rck_port |= tpic_rck_bit; // HIGH*/ 

	   lc.setChar(0, 0, fcu.DisplayString.charAt(idx), false);
	   
	   delayMicroseconds(100);

	   *tpic_rck_port &= ~tpic_rck_bit; // LOW
	   qs.shiftOut(0);
	   qs.shiftOut(1 << idx);
	   *tpic_rck_port |= tpic_rck_bit; // HIGH
   }
   *tpic_rck_port &= ~tpic_rck_bit; // LOW 
   qs.shiftOut(0); qs.shiftOut(0);
   *tpic_rck_port |= tpic_rck_bit; // HIGH*/ 

   lc.shutdown(0, true);
  //activeDevice = !activeDevice;
  
  
}
// called when the client request is complete

int speed = 0;
void sendButtonPacket()
{
	/*Serial.print("Sending packet to IP: ");
	Serial.print(String(serverIP[0], 10) + String(serverIP[1], 10) + String(serverIP[2]) + String(serverIP[3]));
	Serial.print(":");
	Serial.println(SERVER_PORT);*/

	if (udp.beginPacket(serverIP, SERVER_PORT))
	{
		byte magic[]{ 0xff, 0xfa, 0x04, 0x01 };
		

		udp.write(magic, 4);
		uint16_t Buttons = 0;
		uint8_t EncButtons = 0;

		for (int i = 0; i < 8; i++) {
			bitWrite(EncButtons, i, PushButtons[i]->isPressed(false));
		}
		for (int i = 8; i < 17; i++) {
			bitWrite(Buttons, i-8, PushButtons[i]->isPressed(false));
		}

		udp.write(Buttons);
		udp.write(Buttons >> 8);
		
		udp.write(EncButtons);


		udp.endPacket();

		
	}
}
void sendStatusPacket()
{
	// dont send status packet short time after a button is pressed
	// wait for the new values 

	if (!shouldSendStatus) {
		sendButtonPacket(); // this packet gets sent because the server waits for an initial packet
		return;
	}

	if (millis() > dontSendTime + PAUSE_AFTER_BUTTON_PUSH) {

		

		if (udp.beginPacket(serverIP, SERVER_PORT))
		{
			
			signed int iAlt = fcu.Altitude / 1000;
			byte magic[]{ 0xff, 0xfa, 0x04, 0x00 };
			byte speed[]{ fcu.Speed, fcu.Speed >> 8 };
			byte hdg[]{ fcu.Heading, fcu.Heading >> 8 };
			byte alt[]{ (iAlt), (iAlt) >> 8 };
			byte vs[]{ fcu.VerticalSpeed, (fcu.VerticalSpeed) >> 8 };

			udp.write(magic, 4);
			udp.write(speed, 2);
			udp.write(hdg, 2);
			udp.write(alt, 2);
			udp.write(vs, 2);


			udp.endPacket();
		}/*
		Serial.print("Status: ");
		Serial.print(fcu.Speed); Serial.print(" ("); Serial.print(encSPDValue); Serial.print(") | ");
		Serial.print(fcu.Heading); Serial.print(" ("); Serial.print(encHDGValue); Serial.print(") | ");
		Serial.print(fcu.Altitude); Serial.print(" ("); Serial.print(encALTValue); Serial.print(") "); Serial.print(iAlt, 10); Serial.print(" |");
		Serial.print(fcu.VerticalSpeed); Serial.print(" ("); Serial.print(encVSValue); Serial.print(") ");
		Serial.print("'");  Serial.print(fcu.DisplayString); Serial.print("'");
		Serial.println();*/

	}

	// millis has been reset, reset dontSendTime
	if (millis() < dontSendTime) {
		dontSendTime = 0;
	}
}
void readUdp()
{
	int packetSize = udp.parsePacket();
	if (packetSize == 0) return;

	udp.read(udpPacketBuffer, 32);

	
	if (udpPacketBuffer[0] == 0xff && udpPacketBuffer[1] == 0xfa && udpPacketBuffer[2] == 0x04) {
		
		parseUdpPacket();
		
	}
	



}

void parseUdpPacket()
{
	int pos = 3;
	byte msgType = udpPacketBuffer[pos++];
	if (msgType == 0x00) { // LED Message
		


		byte b1 = udpPacketBuffer[pos++];
		byte b2 = udpPacketBuffer[pos];

		fcu.SetSpeedMode((bitRead(b1, 1) == 1) ? FCU::DisplayModeSpeed::MACH : FCU::DisplayModeSpeed::KTS);
		fcu.SetVSMode((bitRead(b1, 2) == 1) ? FCU::DisplayModeVS::FPA : FCU::DisplayModeVS::VS);
		fcu.HDGMode = (bitRead(b1, 3) == 1) ? FCU::Mode::Managed : FCU::Mode::Selected;
		fcu.ALTMode = (bitRead(b1, 4) == 1) ? FCU::Mode::Managed : FCU::Mode::Selected;
		fcu.SPDMode = (bitRead(b1, 5) == 1) ? FCU::Mode::Managed : FCU::Mode::Selected;

		digitalWrite(LED_AP1, (bitRead(b1, 6) == 1));
		digitalWrite(LED_AP2, (bitRead(b1, 7) == 1));

		digitalWrite(LED_ATHR, (bitRead(b2, 0) == 1));
		digitalWrite(LED_LOC, (bitRead(b2, 1) == 1));
		digitalWrite(LED_APPR, (bitRead(b2, 2) == 1));
		digitalWrite(LED_EXPED, (bitRead(b2, 3) == 1));


	}
	else if(msgType == 0x01) { // Display State Message
		int spd = (udpPacketBuffer[pos+1] << 8) | udpPacketBuffer[pos];
		pos += 2;
		int hdg = (udpPacketBuffer[pos + 1] << 8) | udpPacketBuffer[pos];
		pos += 2;
		signed long alt = (udpPacketBuffer[pos + 1] << 8) | udpPacketBuffer[pos];
		pos += 2;
		int vs = (udpPacketBuffer[pos + 1] << 8) | udpPacketBuffer[pos];

		if (spd == 0 && hdg == 0 && alt == 0) { return; }

		// only update values if no button was pressed
		if (millis() > pushTime + LOCK_TIME) {
		
			if (fcu.Speed > 0 && spd < 0) {
				lastDottedSpeed = fcu.Speed;
			}
			if (fcu.Altitude > 0 && alt < 0) {
				lastDottedAlt = fcu.Altitude;
			}
			if (fcu.Heading > 0 && hdg < 0) {
				lastDottedHeading = fcu.Heading;
			}

			
			fcu.Speed = spd;
			if (spd > 0) {
				encSPD.write(spd*ENC_PULSE);
			}

			//fcu.SetEncoderValue(FCU::SPEED, spd, &encSPD);
			fcu.Heading = hdg;
			if (hdg > 0) {
				encHDG.write(hdg*ENC_PULSE);
			}

			
			//fcu.SetEncoderValue(FCU::HEADING, hdg, &encHDG);
			
			fcu.Altitude = (alt > 0) ? alt * 100 : alt;
			
			//fcu.SetEncoderValue(FCU::ALTITUDE, alt*100, &encALT);			
			
			
			
			
			fcu.VerticalSpeed = vs;
			if (vs == -998) {
				fcu.VSDashed = false;
			}
			
			//fcu.SetEncoderValue(FCU::VSPEED, vs, &encVS);
			

		}
		// reset once the millis timer is reset to 0
		if (millis() < pushTime) {
			pushTime = 0;
		}
		
		
		// received first data - send status now
		shouldSendStatus = true; 

		

		fcu.displayDirty = true;
	}


	udpPacketBuffer[0] = 0;
}
void loop () {

	
	
	for (PushButtonCounter = 0; PushButtonCounter < sizeof(ButtonList); PushButtonCounter++)
	{
		PushButtons[PushButtonCounter]->process();
	}
	
	encSPDValue = encSPD.read();
	encHDGValue = encHDG.read();
	encALTValue = encALT.read();
	encVSValue = encVS.read();

	if (encSPDValue != encLastSPDValue) {
		
		fcu.SetEncoderValue(FCU::SPEED, encSPDValue, &encSPD);
		encLastSPDValue = encSPDValue;
		pushTime = millis();
	}
	if (encHDGValue != encLastHDGValue) {
		fcu.SetEncoderValue(FCU::HEADING, encHDGValue, &encHDG);
		encLastHDGValue = encHDGValue;
		pushTime = millis();
	}
	if (encALTValue != encLastALTValue) {
		fcu.SetEncoderValue(FCU::ALTITUDE, encALTValue, &encALT);
		encLastALTValue = encALTValue;
		pushTime = millis();
	}
	if (encVSValue != encLastVSValue) {
		fcu.SetEncoderValue(FCU::VSPEED, encVS.read(), &encVS);
		encLastVSValue = encVSValue;
		pushTime = millis();
	}
	
	
	
	
	networkTimer.run();


	readUdp();
	fcu.UpdateDisplayString();
	if (fcu.VerticalSpeed == -8000) {
		Serial.println("NOW");
		delay(5000);
		softReset();
	}
}
void softReset() {
	__asm__("  jmp 0");
}
	

		



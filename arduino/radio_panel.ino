
// PINS

////STATUS LEDS
int StatusLED_LATCH = 32;
int StatusLED_CLOCK = 34;
int StatusLED_DATA = 33;

//// MODE SWITCHES
const int SwitchColumns = 4;
const int SwitchRows = 4;
const byte SwitchInput[SwitchColumns] = {23, 25, 28, 27}; //array of pins used as output for SwitchRows of keypad
const byte SwitchOutput[SwitchRows] = {22, 24, 29, 26};
int h, v;

// ACTIVE LED
byte ActiveLED_LATCH = 47;
byte ActiveLED_CLOCK = 46;
byte ActiveLED_DATA = 44;

byte ActiveLED_CathDataA[7] = { 0, 0b10101000, 0b10101001, 0b00101001, 0b10001001, 0b10100001, 0b10101001};
byte ActiveLED_CathDataB[7] = { 0, 0b10001011, 0b10001010, 0b10001011, 0b10001011, 0b10001011, 0b10001001};

byte ActiveLED_NumbersA[10] = { 0b01000110, 0b00000010, 0b01010010, 0b01010010, 0b00010110, 0b01010100, 0b01010100, 0b01000010, 0b01010110, 0b01010110 };
byte ActiveLED_NumbersB[10] = { 0b01100100, 0b00000100, 0b01100000, 0b00100100, 0b00000100, 0b00100100, 0b01100100, 0b00000100, 0b01100100, 0b00100100};
byte ActiveLED_DP = 0b00010000;
String ActiveLEDValue = "134.453";

// STANDBY LED
byte StandbyLED_LATCH = A1;
byte StandbyLED_CLOCK = A2;
byte StandbyLED_DATA = A0;

byte StandbyLED_CathDataA[7] = { 0, 0b00010101, 0b00000101, 0b00010001, 0b00010100, 0b00010101, 0b00010101 };
byte StandbyLED_CathDataB[7] = { 0, 0b10000001, 0b10100001, 0b10100001, 0b10100001, 0b00100001, 0b10100000 };

byte StandbyLED_NumbersA[10] = { 0b01001000, 0b01000000, 0b01001010, 0b01001010, 0b01000010, 0b00001010, 0b00001010, 0b01001000, 0b01001010, 0b01001010 };
byte StandbyLED_NumbersB[10] = { 0b01011010, 0b00000010, 0b00011000, 0b00001010, 0b01000010, 0b01001010, 0b01011010, 0b00000010, 0b01011010, 0b01001010 };
byte StandbyLED_DP = 0b00000100;
String StandbyLEDValue = "123.476";
int StandbyLEDValueA = 0;
int StandbyLEDValueB = 0;
unsigned long StandbyUpdateLocked = 0;
unsigned long StandbyLockedTime = 10000;


// Misc Switches
byte SwitchOnOff = A3;
byte SwitchCTSChange = A4;
byte SwitchRotaryHIGH = A5;
byte SwitchRotaryLOW = A6;

byte rotary = 0;
byte rotaryTmp = 0;
byte ctsprev = 0;
byte prevToggle = 0;

bool CTSLeft = false;

// GLOBALS
String c1 = ""; // COM1
String c1s = ""; //COM1 Standby

String n1 = ""; // NAV1
String n1s = ""; //NAV1 Standby

String adf = ""; // ADF

char PacketBuffer[50];
String PacketString;

enum RadioMode {
  VHF1, VHF2, VHF3, HF1, HF2, AM, NAV, VOR, ILS, MLS, ADF, BFO
};

typedef struct {
  RadioMode mode;
  int pinA;
  int pinB;
  String FrequencyActive;
  String FrequencyStandby;

} RMode;
int modeCount = 12;
RadioMode currentMode = BFO;
RadioMode previousMode = BFO;

RMode modeData[12] = {
  { VHF1, 2 << 1, 0, "118.125", "119.250"},
  { VHF2, 2 << 3, 0, "119.000", "124.475"},
  { VHF3, 2 << 6, 0, "121.500", "134.230"},
  { HF1, 2 << 4, 0, "7.160", "2.800"},
  { HF2, 2 << 2, 0, "12.300", "25.150"},
  { AM, 2 << 5, 0, "1000", "840"},
  { NAV, 0, 2 << 1, "110.550", "112.500"},
  { VOR, 0, 2 << 2, "110.550", "118.110"},
  { ILS, 0, 2 << 3, "109.550", "112.230"},
  { MLS, 0, 2 << 5, "100.400", "114.500"},
  { ADF, 0, 2 << 4, "284.0", "330.0"},
  { BFO, 0, 2 << 0, "382.0", "310.5"},
};

byte data;
byte dataArray[10];
bool serialDataReceived = false;
byte SerialDataCount = 0;
unsigned long SerialLastTransmission = 0;
unsigned long SerialTransmitEvery = 1000;
void setup() {


  Serial.begin(9600);

  setupActiveLED();
  setupStandbyLED();
  initLEDs();

  setupModeSwitches();

  setupMiscSwitches();
  setupStatusLED();

  currentMode = previousMode = VHF1;

  SetStandByValue(modeData[currentMode].FrequencyStandby);
  ctsprev = digitalRead(SwitchCTSChange);

}

void loop() {

  tryReadSerial();
  if (digitalRead(SwitchOnOff) == LOW) {
    StatusLEDloop();
    loopModeSwitches();
    loopActiveLED();
    loopStandbyLED();
    loopMiscSwitches();

    SendSerialData();
  } else
  {
    resetAll();
  }


}

void SendSerialData()
{
  if(millis() > SerialLastTransmission + SerialTransmitEvery) {
    String serialString = "$C" + modeData[VHF1].FrequencyActive + "|" + modeData[VHF1].FrequencyStandby + ";";
    serialString += "N" + modeData[NAV].FrequencyActive + "|" + modeData[NAV].FrequencyStandby + ";";
    serialString += "A" + modeData[ADF].FrequencyActive + ";";
    SerialLastTransmission = millis();
    Serial.println(serialString);
  }
}

void resetAll()
{
  digitalWrite(StandbyLED_LATCH, 0);
  shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, 0);
  shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, 0);
  digitalWrite(StandbyLED_LATCH, 1);

  digitalWrite(ActiveLED_LATCH, 0);
  shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, 0);
  shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, 0);
  digitalWrite(ActiveLED_LATCH, 1);
  digitalWrite(StatusLED_LATCH, 0);
  shiftOut(StatusLED_DATA, StatusLED_CLOCK, 0);
  shiftOut(StatusLED_DATA, StatusLED_CLOCK, 0);
  digitalWrite(StatusLED_LATCH, 1);
}



void initLEDs()
{
  digitalWrite(StandbyLED_LATCH, 0);
  shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, 0b00000010);
  shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, 0);
  digitalWrite(StandbyLED_LATCH, 1);

  digitalWrite(ActiveLED_LATCH, 0);
  shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, 0b00010000);
  shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, 0);
  digitalWrite(ActiveLED_LATCH, 1);
}

void SetStandByValue(String val)
{
  StandbyLEDValue = val;
  val.replace(".", "");
  StandbyLEDValueA = atoi(val.substring(0, 3).c_str());
  StandbyLEDValueB = atoi(val.substring(3, 6).c_str());
}


/* ADDITIONAL SWITCHES (ON/OFF CTS)
 *
 */

void setupMiscSwitches() {
  pinMode(A3, INPUT_PULLUP);

  pinMode(A4, INPUT_PULLUP);

  pinMode(A5, INPUT_PULLUP);

  pinMode(A6, INPUT_PULLUP);

  rotary = getRotaryValue();

}
int getRotaryValue()
{
  rotaryTmp = 0;
  rotaryTmp |= 1 << digitalRead(A5);
  rotaryTmp |= 2 << digitalRead(A6);
  return rotaryTmp;
}
void loopMiscSwitches()
{
  byte ctsChangeValue = digitalRead(SwitchCTSChange);

  if (ctsChangeValue != ctsprev) {
    if ( ctsChangeValue == LOW ) {
      CTSLeft = !CTSLeft;
    }

    ctsprev = ctsChangeValue;

  }

  rotaryTmp = getRotaryValue();



  if (rotaryTmp == rotary) {
    return;
  }

  if (rotary == 2 && rotaryTmp == 6) {
    LEDstep(false);
    rotary = rotaryTmp;
    return;
  }
  if (rotary == 6 && rotaryTmp == 2) {
    LEDstep(true);
    rotary = rotaryTmp;
    return;
  }
  if (rotaryTmp < rotary) {
    LEDstep(false);
    rotary = rotaryTmp;
    return;
  }
  if (rotaryTmp > rotary) {
    LEDstep(true);
    rotary = rotaryTmp;
    return;
  }



}

void LEDstep( bool up)
{
  switch (currentMode) {
    case VHF1:
      if (CTSLeft) {
        StandbyLEDValueA += (up) ? 1 : -1;

        if (StandbyLEDValueA > 136) {
          StandbyLEDValueA = 118;
        }
        if (StandbyLEDValueA < 118) {
          StandbyLEDValueB = 136;
        }
      } else {

        StandbyLEDValueB += (up) ? 25 : -25;

        if (StandbyLEDValueB > 999) {
          StandbyLEDValueB = 0;
        }
        if (StandbyLEDValueB < 0) {
          StandbyLEDValueB = 975;
        }
      }

      StandbyUpdateLocked = millis();
      
      modeData[currentMode].FrequencyStandby = formatVHF();
      SetStandByValue(modeData[currentMode].FrequencyStandby);
      break;
  }
}
String formatVHF()
{
  String f = String(StandbyLEDValueA) + ".";
  if (StandbyLEDValueB < 100) {
    f += "0";
  }
  if (StandbyLEDValueB < 10) {
    f += "0";
  }
  f += String(StandbyLEDValueB);
  return f;
}

/* 7-Seg LED STANDBY */

void setupStandbyLED() {

  pinMode(StandbyLED_LATCH, OUTPUT);
  pinMode(StandbyLED_CLOCK, OUTPUT);
  pinMode(StandbyLED_DATA, OUTPUT);


}

void loopStandbyLED()
{
  byte hasPoint = false;
  byte p = 0;
  String s = modeData[currentMode].FrequencyStandby;
  s.replace(".", "");

  int len = modeData[currentMode].FrequencyStandby.length();
  while (s.length() < 6) {
    modeData[currentMode].FrequencyStandby = "-" + modeData[currentMode].FrequencyStandby;
    s = "-" + s;
  }

  byte place = 1;
  for (int i = 0; i < len; i++) {
    int c = atoi(String(modeData[currentMode].FrequencyStandby[i]).c_str());
    if ( modeData[currentMode].FrequencyStandby[i + 1] == '.') {
      p = StandbyLED_DP;
    } else {
      p = 0;
    }
    if (modeData[currentMode].FrequencyStandby[i] == '.') {
      continue;
    }
    if (modeData[currentMode].FrequencyStandby[i] == '-') {
      place++;
      continue;
    }
    digitalWrite(StandbyLED_LATCH, 0);
    shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, StandbyLED_CathDataA[place] | StandbyLED_NumbersA[c]);
    shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, StandbyLED_CathDataB[place] | StandbyLED_NumbersB[c] | p);
    digitalWrite(StandbyLED_LATCH, 1);

    place++;
  }

  digitalWrite(StandbyLED_LATCH, 0);
  shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, 0);
  shiftOut(StandbyLED_DATA, StandbyLED_CLOCK, 0);
  digitalWrite(StandbyLED_LATCH, 1);

}


/* 7-Seg LED ACTIVE */

void setupActiveLED() {

  pinMode(ActiveLED_LATCH, OUTPUT);
  pinMode(ActiveLED_CLOCK, OUTPUT);
  pinMode(ActiveLED_DATA, OUTPUT);



}

void loopActiveLED()
{
  digitalWrite(ActiveLED_LATCH, 0);
  digitalWrite(ActiveLED_LATCH, 1);

  byte hasPoint = false;
  byte p = 0;
  String s = modeData[currentMode].FrequencyActive;
  s.replace(".", "");
  int len = modeData[currentMode].FrequencyActive.length();



  while (s.length() < 6) {
    modeData[currentMode].FrequencyActive = "-" + modeData[currentMode].FrequencyActive;
    s = "-" + s;
  }

  byte place = 1;
  for (int i = 0; i < len; i++) {
    int c = atoi(String(modeData[currentMode].FrequencyActive[i]).c_str());

    if ( modeData[currentMode].FrequencyActive[i + 1] == '.') {
      p = ActiveLED_DP;
    } else {
      p = 0;
    }
    if (modeData[currentMode].FrequencyActive[i] == '-') {
      place++;
      continue;
    }
    if (modeData[currentMode].FrequencyActive[i] == '.') {
      continue;
    }
    digitalWrite(ActiveLED_LATCH, 0);
    shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, ActiveLED_CathDataA[place] | ActiveLED_NumbersA[c]);
    shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, ActiveLED_CathDataB[place] | ActiveLED_NumbersB[c] | p);
    digitalWrite(ActiveLED_LATCH, 1);
    //delay(50);
    place++;
  }
  digitalWrite(ActiveLED_LATCH, 0);
  shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, 0);
  shiftOut(ActiveLED_DATA, ActiveLED_CLOCK, 0);
  digitalWrite(ActiveLED_LATCH, 1);

}

/*****************MODE SWITCHES**********************/

void setupModeSwitches()
{
  for (byte i = 0; i < SwitchRows; i++)
  {
    pinMode(SwitchOutput[i], OUTPUT);
  }
  for (byte s = 0; s < SwitchColumns; s++) //for loop used to makk pin mode of inputs as inputpullup
  {
    pinMode(SwitchInput[s], INPUT_PULLUP);
  }
}

void loopModeSwitches()
{
  byte c = keypad();
  if (c == 50) {

    return;
  }

  Serial.println(c);

  String mSave;


  switch (c)
  {
    case 0:
      currentMode = VHF1;
      break;
    case 4:
      currentMode = VHF2;
      break;
    case 8:
      currentMode = VHF3;
      break;
    case 12:
      StandbyUpdateLocked = millis();
      mSave = modeData[currentMode].FrequencyActive;
      modeData[currentMode].FrequencyActive = modeData[currentMode].FrequencyStandby;
      modeData[currentMode].FrequencyStandby = mSave;

      SetStandByValue(mSave);
      
      break;
    case 1:
      currentMode = HF1;
      break;
    case 9:
      currentMode = HF2;
      break;
    case 13:
      currentMode = AM;
      break;
    case 2:
      currentMode = NAV;
      break;
    case 6:
      currentMode = VOR;
      break;
    case 10:
      currentMode = ILS;
      break;
    case 14:
      currentMode = MLS;
      break;
    case 11:
      currentMode = ADF;
      break;
    case 15:
      currentMode = BFO;
      break;
  }

  //if (previousMode == currentMode) {
  //  return;
  //}
  previousMode = currentMode;

  ActiveLEDValue = modeData[currentMode].FrequencyActive;
  StandbyLEDValue = modeData[currentMode].FrequencyStandby;
  SetStandByValue(modeData[currentMode].FrequencyStandby);


}

byte keypad() // function used to detect which button is used
{
  static bool no_press_flag = 0; //static flag used to ensure no button is pressed
  for (byte x = 0; x < SwitchColumns; x++) // for loop used to read all inputs of keypad to ensure no button is pressed
  {
    if (digitalRead(SwitchInput[x]) == HIGH); //read evry input if high continue else break;
    else
      break;
    if (x == (SwitchColumns - 1)) //if no button is pressed
    {
      no_press_flag = 1;
      h = 0;
      v = 0;
    }
  }
  if (no_press_flag == 1) //if no button is pressed
  {
    for (byte r = 0; r < SwitchRows; r++) //for loop used to make all output as low
      digitalWrite(SwitchOutput[r], LOW);
    for (int h = 0; h < SwitchColumns; h++) // for loop to check if one of inputs is low
    {
      if (digitalRead(SwitchInput[h]) == HIGH) //if specific input is remain high (no press on it) continue
        continue;
      else    //if one of inputs is low
      {
        for (int v = 0; v < SwitchRows; v++) //for loop used to specify the number of row
        {
          digitalWrite(SwitchOutput[v], HIGH);  //make specified output as HIGH
          if (digitalRead(SwitchInput[h]) == HIGH) //if the input that selected from first sor loop is change to high
          {
            no_press_flag = 0;              //reset the no press flag;
            for (byte w = 0; w < SwitchRows; w++) // make all outputs as low
              digitalWrite(SwitchOutput[w], LOW);
            //Serial.println((v*4+h));
            return v * 4 + h; //return number of button
          }
        }
      }
    }
  }
  return 50;
}

////// Status LEDS ///////
void setupStatusLED() {

  pinMode(StatusLED_LATCH, OUTPUT);
  pinMode(StatusLED_CLOCK, OUTPUT);
  pinMode(StatusLED_DATA, OUTPUT);

  blinkAll_2Bytes(2, 50);
  for (int i = 0; i < modeCount; i++) {
    digitalWrite(StatusLED_LATCH, 0);
    shiftOut(StatusLED_DATA, StatusLED_CLOCK, modeData[i].pinA);
    shiftOut(StatusLED_DATA, StatusLED_CLOCK, modeData[i].pinB);
    digitalWrite(StatusLED_LATCH, 1);
    delay(70);
  }

}

void StatusLEDloop() {
  digitalWrite(StatusLED_LATCH, 0);
  shiftOut(StatusLED_DATA, StatusLED_CLOCK, modeData[currentMode].pinA);
  shiftOut(StatusLED_DATA, StatusLED_CLOCK, modeData[currentMode].pinB);
  digitalWrite(StatusLED_LATCH, 1);
}



// the heart of the program
void shiftOut(int myStatusLED_DATA, int myStatusLED_CLOCK, byte myDataOut) {
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i = 0;
  int pinState;
  pinMode(myStatusLED_CLOCK, OUTPUT);
  pinMode(myStatusLED_DATA, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myStatusLED_DATA, 0);
  digitalWrite(myStatusLED_CLOCK, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
  for (i = 7; i >= 0; i--)  {
    digitalWrite(myStatusLED_CLOCK, 0);

    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myStatusLED_DATA, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myStatusLED_CLOCK, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myStatusLED_DATA, 0);
  }

  //stop shifting
  digitalWrite(myStatusLED_CLOCK, 0);
}


//blinks the whole register based on the number of times you want to
//blink "n" and the pause between them "d"
//starts with a moment of darkness to make sure the first blink
//has its full visual effect.
void blinkAll_2Bytes(int n, int d) {
  digitalWrite(StatusLED_LATCH, 0);
  shiftOut(StatusLED_DATA, StatusLED_CLOCK, 0);
  shiftOut(StatusLED_DATA, StatusLED_CLOCK, 0);
  digitalWrite(StatusLED_LATCH, 1);
  delay(200);
  for (int x = 0; x < n; x++) {
    digitalWrite(StatusLED_LATCH, 0);
    shiftOut(StatusLED_DATA, StatusLED_CLOCK, 255);
    shiftOut(StatusLED_DATA, StatusLED_CLOCK, 255);
    digitalWrite(StatusLED_LATCH, 1);
    delay(d);
    digitalWrite(StatusLED_LATCH, 0);
    shiftOut(StatusLED_DATA, StatusLED_CLOCK, 0);
    shiftOut(StatusLED_DATA, StatusLED_CLOCK, 0);
    digitalWrite(StatusLED_LATCH, 1);
    delay(d);
  }
}




void tryReadSerial()
{
  int num = 0;

  if (Serial.available() > 0)
  {
    byte c = Serial.read();
   
    PacketBuffer[SerialDataCount] = (char)c;
    
    SerialDataCount++;
    //int num = Serial.readBytesUntil(';', PacketBuffer, 50);
    
    if (c == ';') {
      
          
      if (SerialDataCount > 0 )
      {
        PacketString = String(PacketBuffer);
        PacketString.trim();
        int pipePos = PacketString.indexOf('|');
        
        switch (PacketBuffer[0])
        {
          case 'C':
            
            c1 = ConvertRadioString(PacketString.substring(1, pipePos));
            c1s = ConvertRadioString(PacketString.substring(pipePos+1, PacketString.indexOf(';')));
            
            if( millis() > StandbyUpdateLocked + StandbyLockedTime) {
              modeData[VHF1].FrequencyStandby = c1s;
              modeData[VHF1].FrequencyActive = c1;
              SetStandByValue(modeData[VHF1].FrequencyStandby);
            }


            break;
          case 'N':
            n1 = PacketString.substring(1, pipePos);

            n1s = PacketString.substring(pipePos+1, PacketString.indexOf(';'));
            
            if( millis() > StandbyUpdateLocked + StandbyLockedTime) {
              modeData[NAV].FrequencyActive = n1;
              modeData[NAV].FrequencyStandby = n1s;
              SetStandByValue(modeData[VHF1].FrequencyStandby);
            }
            break;
          case 'A':
            adf = PacketString.substring(1, PacketString.length());
            modeData[ADF].FrequencyActive = adf;
            break;
        }
        for( int i = 0; i < sizeof(PacketBuffer);i++)
          PacketBuffer[i] = (char)0;
          
        SerialDataCount = 0;

      }
    }
  }

}

/////////////////// HELPER /////////////////
String ConvertRadioString(String r)
{

  if (r.endsWith("2") || r.endsWith("7")) {
    r.concat("5");
  } else {
    r.concat("0");
  }
  return r;


}


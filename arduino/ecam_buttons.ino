#include <Keypad.h>
#include <SimpleTimer.h>

#define LED_FUEL 22
#define LED_CLRB 23
#define LED_HYD 24
#define LED_FCTL 25
#define LED_ELEC 26
#define LED_WHEEL 27
#define LED_DOOR 28
#define LED_PRESS 29
#define LED_APU 30
#define LED_BLEED 31
#define LED_STS 32
#define LED_CLRA 33
#define LED_ENG 34
#define LED_COND 35
#define DEBUG


byte LEDPINS[14] = { LED_FUEL, LED_CLRB, LED_HYD, LED_FCTL, LED_ELEC, LED_WHEEL, LED_DOOR, LED_PRESS, LED_APU, LED_BLEED, LED_STS, LED_CLRA, LED_ENG, LED_COND };
#define AXIS_UPPER A0
#define AXIS_LOWER A1

#define NUM_BUTTONS  40
#define NUM_AXES  8        // 8 axes, X, Y, Z, etc

#define IDENTIFY_AS "ECP"
#define IDENTIFY_INTERVAL 8000


byte h=0,v=0;    //variables used in for loops
const byte rows=3;             //number of rows of keypad
const byte columns=6;            //number of columnss of keypad

byte Output[rows]={44,46,47}; //array of pins used as output for rows of keypad
byte Input[columns]={48,49,50,51,52,53}; //array of pins used as input for columnss of keypad

char PacketBuffer[50];
int SerialDataCount;
String PacketString;

/*char keys[rows][columns] = {
  {64,65,66},{67,68,69},
  {70,71,72},{73,74,75},
  {76,77,78},{79,80,81}
};*/
char keys[rows][columns] = {
  {1,2,3,4,5,6},
  {7,8,9,10,11,12},
  {13,14,15,16,17,18}
};

Keypad kpd = Keypad( makeKeymap(keys), Output, Input, rows, columns);
SimpleTimer IdentifyTimer;


typedef struct joyReport_t {
    int16_t axis[NUM_AXES];
    uint8_t button[(NUM_BUTTONS+7)/8]; // 8 buttons per byte
} joyReport_t;

joyReport_t joyReport;

void setButton(joyReport_t *joy, uint8_t button);
void clearButton(joyReport_t *joy, uint8_t button);
void sendJoyReport(joyReport_t *report);

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  for(int i = 0; i < 14 ; i++)
  {
    pinMode(LEDPINS[i], OUTPUT);
   
  }
  
    Serial.begin(9600);
    delay(200);
    for (uint8_t ind=0; ind<8; ind++) {
      joyReport.axis[ind] =0;
    }
    for (uint8_t ind=0; ind<sizeof(joyReport.button); ind++) {
        joyReport.button[ind] = 0;
    }

    for(int i = 46; i <= 54; i++)
  {
   pinMode(i, INPUT_PULLUP);
  }

  kpd.addEventListener(keypadEvent);
  IdentifyTimer.setInterval(IDENTIFY_INTERVAL, TimerIdentify);
  TimerIdentify();
}

void TimerIdentify()
{
 Serial.write("_!");
 Serial.write(IDENTIFY_AS);
 Serial.write("!_"); 
}
void keypadEvent(KeypadEvent key)
{
  switch(kpd.getState())
  {
    case PRESSED:
      sendKeySerial((int)key, true);
      break;
    case RELEASED:
      sendKeySerial((int)key, false);
      break;
    
    
  }
}
void sendKeySerial(int key, bool press)
{
  int t = 0;
  switch(key)
  {
    case 7: // TO CONFIG
      t = (press) ? 3 : 47; break;
    case 12: // EMERG CANC
      t = (press) ? 18 : 49; break;
    case 16: // DOOR
      t = (press) ? 4 : 62; break;
     case 2: //  ENG
      t = (press) ? 5 : 63; break;
     case 1: // BLEED
      t = (press) ? 6 : 64; break;
     case 4: // PRESS
      t = (press) ? 7 : 65; break;
     case 3: // ELEC
      t = (press) ? 8 : 66; break;
      case 6: // HYD
      t = (press) ? 9 : 67; break;
      case 5: // FUEL
      t = (press) ? 10 : 68; break;
      case 14: // APU
      t = (press) ? 11 : 69; break;
      case 13: // COND
      t = (press) ? 12 : 70; break;
      case 15: // WHEEL
      t = (press) ? 13 : 71; break;
      case 18: // FCTL
      t = (press) ? 14 :72 ; break;
      case 11:
      case 8: // CLR
      t = (press) ? 15 : 73; break;
      case 10: // STS
      t = (press) ? 17 : 74; break;
      case 9: // RCL
      t = (press) ? 16 : 48; break;
      case 17: // ALL
      t = (press) ? 75 : 76; break;
  }
  Serial.print("%");
  Serial.print(t);
  Serial.print("#");
}
void loop() {
  joyReport.axis[0] = (int)((float)(analogRead(AXIS_UPPER) / (float)1024) * (float)65535) - (float)32767;
  joyReport.axis[1] = (int)((float)(analogRead(AXIS_LOWER) / (float)1024) * (float)65535) - (float)32767;
  

  kpd.getKey();
  tryReadSerial();
  IdentifyTimer.run();
}

// Send an HID report to the USB interface
void sendJoyReport(struct joyReport_t *report)
{
#ifndef DEBUG
    Serial.write((uint8_t *)report, sizeof(joyReport_t));
#else
  //Serial.print("%");
  
   
#endif

}
// turn a button on
void setButton(joyReport_t *joy, uint8_t button)
{
    uint8_t index = button/8;
    uint8_t bit = button - 8*index;

    joy->button[index] |= 1 << bit;
}

// turn a button off
void clearButton(joyReport_t *joy, uint8_t button)
{
    uint8_t index = button/8;
    uint8_t bit = button - 8*index;

    joy->button[index] &= ~(1 << bit);
}

byte keypad() // function used to detect which button is used 
{
 static bool no_press_flag=0;  //static flag used to ensure no button is pressed 
  for(byte x=0;x<columns;x++)   // for loop used to read all inputs of keypad to ensure no button is pressed 
  {
 
     
     if (digitalRead(Input[x])==HIGH);   //read evry input if high continue else break;
     else
     break;
     if(x==(columns-1))   //if no button is pressed 
     {
      no_press_flag=1;
      h=0;
      v=0;
     }
  }
  
  if(no_press_flag==1) //if no button is pressed 
  {
    for(byte r=0;r<rows;r++) //for loop used to make all output as low
    digitalWrite(Output[r],LOW);
    
    for(h=0;h<columns;h++)  // for loop to check if one of inputs is low
    {
      if(digitalRead(Input[h])==HIGH) //if specific input is remain high (no press on it) continue
      continue;
      else    //if one of inputs is low
      {
          for (v=0;v<rows;v++)   //for loop used to specify the number of row
          {
          digitalWrite(Output[v],HIGH);   //make specified output as HIGH
          if(digitalRead(Input[h])==HIGH)  //if the input that selected from first sor loop is change to high
          {
            no_press_flag=0;                //reset the no press flag;
            for(byte w=0;w<rows;w++) // make all outputs as low
            digitalWrite(Output[w],LOW);
            return v*4+h;  //return number of button 
          }
          }
      }
    }
  }
 return 50;
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
    
    if (c == '#') {

      if (SerialDataCount > 0 )
      {
        PacketString = String(PacketBuffer);
        PacketString.trim();
        PacketString = PacketString.substring(1);
        PacketString.replace('#','\0');
        
        int k = PacketString.indexOf(',');
        String sf = PacketString.substring(0,k);
        int first = atoi(PacketString.substring(0,k).c_str());
        int second = atoi(PacketString.substring(k+1).c_str());

        digitalWrite(LED_DOOR, ( first & 1 << 1));
        digitalWrite(LED_ENG, ( first & 1 << 2));
        digitalWrite(LED_BLEED, ( first & 1 << 3));
        digitalWrite(LED_PRESS, ( first & 1 << 4));
        digitalWrite(LED_ELEC, ( first & 1 << 5));
        digitalWrite(LED_HYD, ( first & 1 << 6));
        digitalWrite(LED_FUEL, ( first & 1 << 7));

        digitalWrite(LED_APU, ( second & 1 << 0));
        digitalWrite(LED_COND, ( second & 1 << 1));
        digitalWrite(LED_WHEEL, ( second & 1 << 2));
        digitalWrite(LED_FCTL, ( second & 1 << 3));
        digitalWrite(LED_CLRA, ( second & 1 << 4));
        digitalWrite(LED_CLRB, ( second & 1 << 4));
        digitalWrite(LED_STS, ( second & 1 << 5));
        

        for ( int i = 0; i < sizeof(PacketBuffer); i++)
          PacketBuffer[i] = (char)0;

        SerialDataCount = 0;
       
      }
    }
  
  }
}


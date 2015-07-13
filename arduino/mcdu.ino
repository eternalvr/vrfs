#include <Keypad.h>
#define DIR 1
#define PROG 2
#define PERF 3
#define INIT 4
#define DATA 5
#define FPLN 6
#define RADNAV 7
#define FUELPRED 8
#define SECFLPN 9
#define FIX 10
#define MCDUMENU 11
#define AIRPORT 12
#define UP 30
#define DOWN 14
#define NEXTPAGE 15
#define OVFY 16
#define CLR 17
#define LSK1L 18
#define LSK2L 19
#define LSK3L 20
#define LSK4L 21
#define LSK5L 22
#define LSK6L 23
#define LSK1R 24
#define LSK2R 25
#define LSK3R 26
#define LSK4R 27
#define LSK5R 28
#define LSK6R 29

int BGLights[] = { 0, 1,2,3,4,5,6,7 };
byte h=0,v=0;    //variables used in for loops
const byte rows=8;             //number of rows of keypad
const byte columns=14;            //number of columnss of keypad
byte Output[rows]={24,25,26,27,28,29,30,31}; //array of pins used as output for rows of keypad
byte Input[columns]={32,33,34,35,36,37,38,39, 46, 47, 48, 49, 50, 51}; //array of pins used as input for columnss of keypad

char keys[rows][columns] = {
  {0,0,0,0, '2', '5', '8', '0', 0, 0, 0, 0, 0, 0},
  {FPLN,DIR,AIRPORT,NEXTPAGE,'1','4','7','.',LSK2L,LSK1L,LSK4L,LSK3L,LSK6L,LSK5L},
  {FUELPRED,PERF,'A','F','K','P','U','Z',36,37,38,39,40,41},
  {RADNAV,PROG,UP,DOWN,'3','6','9','/',50,51,52,53,54,55},
  {FIX,DATA,'C','H','M','R','W','-',64,65,66,67,68,69},
  {SECFLPN,INIT,'B','G','L','Q','V','+',78,79,80,81,82,83},
  {MCDUMENU,85,'E','J','O','T','Y',CLR,LSK2R,LSK1R,LSK4R,LSK3R,LSK6R,LSK5R},
  {98,99,'D','I','N','S','X',OVFY,107,108,109,110,111,112}
};

Keypad kpd = Keypad( makeKeymap(keys), Output, Input, rows, columns);

void setup() {
  for(int i = 0;i < 8; i++)
  {
    pinMode(BGLights[i], OUTPUT);
    
    digitalWrite(BGLights[i], HIGH);
  }
  Serial.begin(9600); //to use serial monitor we set the buad rate

}

void loop() {
  char key = kpd.getKey();
  if(key != NO_KEY) {
    Serial.print("!");
    Serial.println(key);
  }
  //int c = keypad();
 /* if(c != 50)
  {
    Serial.println(c);
  }*/
  

}

byte keypad() // function used to detect which button is used 
{
 static bool no_press_flag=0;  //static flag used to ensure no button is pressed 
  for(byte x=0;x<columns;x++)   // for loop used to read all inputs of keypad to ensure no button is pressed 
  {
 Serial.print(digitalRead(Input[x]));
     
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
  Serial.println();
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

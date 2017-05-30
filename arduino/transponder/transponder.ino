#include <SevenSeg.h>
#include <AdvButton.h>
#include <ButtonManager.h>
#include <SimpleTimer.h>

#define ATC_FAIL_LED A1
#define BASE_GND 30

#define SW_ALTRPTG_ON A0
#define SW_ALTRPTG_OFF A3

#define SW_ATC_1 A5
#define SW_ATC_2 A2

#define SW_ATC_STBY A6
#define SW_ATC_AUTO A7
#define SW_ATC_ON A4

#define KEY_MIN 22
#define KEY_MAX 32

#define KEY_1 31
#define KEY_2 26
#define KEY_3 25
#define KEY_4 28
#define KEY_5 27
#define KEY_6 22
#define KEY_7 29
#define KEY_0 24
#define KEY_CLR 23

#define SW_TCAS_MIN 40
#define SW_TCAS_MAX 48

#define SW_TCAS_THRT 47
#define SW_TCAS_ALL 44
#define SW_TCAS_ABV 45
#define SW_TCAS_BLW 42

#define SW_TCAS_STBY 43
#define SW_TCAS_TA 40
#define SW_TCAS_TARA 41

#define SW_IDENT 46

#define LED_CATH_1 5
#define LED_CATH_2 4
#define LED_CATH_3 3
#define LED_CATH_4 2

typedef struct transponder_report_t {
  byte Buttons[3];
  
  byte Transponder[4];
};
transponder_report_t Report;
transponder_report_t prevReport;

bool editingTransponder = false;
int editingIndex = 0;

String transponderSetting = "0000";
String newTransponderSetting = "";
const int numOfDigits = 4;
int digitPins[numOfDigits] = {5,2,3,4};
SevenSeg disp(36,39,32,33, 35,34,37);

#define IDENTIFY_AS "TCAS"
#define IDENTIFY_INTERVAL 8000
SimpleTimer IdentifyTimer;

AdvButton Key1 = AdvButton(KEY_1, OnKeypadPress, 10000,10000, btn_Digital);
AdvButton Key2 = AdvButton(KEY_2, OnKeypadPress, 10000,10000, btn_Digital);
AdvButton Key3 = AdvButton(KEY_3, OnKeypadPress, 10000,10000, btn_Digital);

AdvButton Key4 = AdvButton(KEY_4, OnKeypadPress, 10000,10000, btn_Digital);
AdvButton Key5 = AdvButton(KEY_5, OnKeypadPress, 10000,10000, btn_Digital);
AdvButton Key6 = AdvButton(KEY_6, OnKeypadPress, 10000,10000, btn_Digital);

AdvButton Key7 = AdvButton(KEY_7, OnKeypadPress, 10000,10000, btn_Digital);
AdvButton Key0 = AdvButton(KEY_0, OnKeypadPress, 10000,10000, btn_Digital);
AdvButton KeyCLR = AdvButton(KEY_CLR, OnKeypadPress, 10000,10000, btn_Digital);

void setup() {
  pinMode(ATC_FAIL_LED, OUTPUT);
  digitalWrite(ATC_FAIL_LED, HIGH);

  pinMode(BASE_GND, OUTPUT);
  digitalWrite(BASE_GND, LOW);
  Serial.begin(9600);

  pinMode(SW_ALTRPTG_ON, INPUT_PULLUP);
  pinMode(SW_ALTRPTG_OFF, INPUT_PULLUP);
  pinMode(SW_ATC_1, INPUT_PULLUP);
  pinMode(SW_ATC_2, INPUT_PULLUP);
  pinMode(SW_ATC_STBY, INPUT_PULLUP);
  pinMode(SW_ATC_AUTO, INPUT_PULLUP);
  pinMode(SW_ATC_ON, INPUT_PULLUP);
  
  for(int i = KEY_MIN;i < KEY_MAX; i++)
  {
     if(i == BASE_GND)continue;
    pinMode(i, INPUT_PULLUP);  
  }
  for(int j = SW_TCAS_MIN; j < SW_TCAS_MAX; j++)
  {
    pinMode(j, INPUT_PULLUP);
  }

  disp.setDigitPins(numOfDigits, digitPins);
  disp.setCommonCathode();
  disp.setTimer(2);
  disp.startTimer();
  disp.write(transponderSetting);

  IdentifyTimer.setInterval(IDENTIFY_INTERVAL, TimerIdentify);
  TimerIdentify();

  digitalWrite(ATC_FAIL_LED, LOW);
  delay(100);
  digitalWrite(ATC_FAIL_LED, HIGH);
  delay(100);
  digitalWrite(ATC_FAIL_LED, LOW);
  delay(100);
}
void checkButtons()
{
  bitWrite(Report.Buttons[0],0,!digitalRead(SW_IDENT));
  bitWrite(Report.Buttons[0],1,!digitalRead(SW_ALTRPTG_ON));
  bitWrite(Report.Buttons[0],2,!digitalRead(SW_ALTRPTG_OFF));
  bitWrite(Report.Buttons[0],3,!digitalRead(SW_ATC_2));
  bitWrite(Report.Buttons[0],4,!digitalRead(SW_ATC_1));
  bitWrite(Report.Buttons[0],5,!digitalRead(SW_ATC_ON));
  bitWrite(Report.Buttons[0],6,!digitalRead(SW_ATC_AUTO));
  bitWrite(Report.Buttons[2],0,!digitalRead(SW_ATC_STBY));
    
  bitWrite(Report.Buttons[1],0,!digitalRead(SW_TCAS_TARA));
  bitWrite(Report.Buttons[1],1,!digitalRead(SW_TCAS_TA));
  bitWrite(Report.Buttons[1],2,!digitalRead(SW_TCAS_STBY));
  bitWrite(Report.Buttons[1],3,!digitalRead(SW_TCAS_BLW));
  bitWrite(Report.Buttons[1],4,!digitalRead(SW_TCAS_ABV));
  bitWrite(Report.Buttons[1],5,!digitalRead(SW_TCAS_ALL));
  bitWrite(Report.Buttons[1],6,!digitalRead(SW_TCAS_THRT));

  for(int i = 0; i < 4; i++)
  {
    Report.Transponder[i] = transponderSetting[i];
  }
  
  
}
void TimerIdentify()
{
 Serial.write("_!");
 Serial.write(IDENTIFY_AS);
 Serial.write("!_"); 
}

void OnKeypadPress(AdvButton* but)
{
  if(!editingTransponder) {
    editingTransponder = true;
    editingIndex = 0;
  }
  char c;
  switch(but->pin)
  {
    case KEY_1: c='1';break;
    case KEY_2: c='2';break;
    case KEY_3: c='3';break;
    case KEY_4: c='4';break;
    case KEY_5: c='5';break;
    case KEY_6: c='6';break;
    case KEY_7: c='7';break;
    case KEY_0: c='0';break;
    case KEY_CLR: c='+';break;
  }
  if(c!='+' && newTransponderSetting.length() < 4 ) {  
    newTransponderSetting+=c;
  }
  if(newTransponderSetting.length() == 4) {
    transponderSetting = newTransponderSetting;
    newTransponderSetting = "";
  }
  if(c=='+') {
    newTransponderSetting = "";
  }
  
  String copy;
  for(int i = 0; i < 4; i++)
  {
    copy += (newTransponderSetting[i] != 0) ? newTransponderSetting[i] : transponderSetting[i];
  }
  disp.write(copy);

  
  
}
void sendReport(struct transponder_report_t *report)
{
    
    if (memcmp( report, &prevReport, sizeof( transponder_report_t ) ) != 0)
    {
      Serial.print("[");
      Serial.write((byte *)report, sizeof(transponder_report_t));    
      Serial.print("#");
      memcpy ( &prevReport, report, sizeof( transponder_report_t ) );
    }
  
  
}
void loop() {
  ButtonManager::instance()->checkButtons();
  IdentifyTimer.run();
  checkButtons();
  sendReport(&Report);
}
ISR(TIMER2_COMPA_vect){
  disp.interruptAction();
}



uint8_t dec2bcd2(uint8_t n)  // takes 2244 usec for 1000 calls
{
  uint16_t a = n;
  byte b = (a*103) >> 10;  // this equals:  b = a/10; 
  return  n + b*6;
}

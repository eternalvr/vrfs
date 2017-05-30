#define LED_FIRE_ENG1 2
#define LED_FAULT_ENG1 3
#define LED_FIRE_ENG2 4
#define LED_FAULT_ENG2 5

#define SW_NORM 6
#define SW_CRANK 7
#define SW_IGN 8
#define SW_ENG1 A0
#define SW_ENG2 A1

#define SW_MASTER1 9
#define SW_MASTER2 A3

#define SPOILERS A5
#define SPOILERS_MAX 1023
#define SPOILERS_MIN 840

#define FLAPS A4
#define FLAPS_MAX 560
#define FLAPS_MIN 320

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_FIRE_ENG1, OUTPUT);
  pinMode(LED_FIRE_ENG2, OUTPUT);
  pinMode(LED_FAULT_ENG1, OUTPUT);
  pinMode(LED_FAULT_ENG2, OUTPUT);

  for (int i = 6; i <= 9; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  Serial.begin(9600);
  Joystick.begin();
}

int ConvertToValue(int value, int vmin, int vmax)
{
  float t = (((float)(value - vmin) / (float)(vmax - vmin)) * 255) - 127;


  return (int)t;
}

void loop() {


  //Serial.println(analogRead(SPOILERS));
  //Serial.println(analogRead(FLAPS));

  Joystick.setXAxis(ConvertToValue(analogRead(SPOILERS), SPOILERS_MIN, SPOILERS_MAX));
  Joystick.setYAxis(ConvertToValue(analogRead(FLAPS), FLAPS_MIN, FLAPS_MAX));
  Joystick.setButton(0, (digitalRead(SW_ENG1) == LOW) ? 1 : 0);
  Joystick.setButton(1, (digitalRead(SW_ENG2) == LOW) ? 1 : 0);
  Joystick.setButton(2, (digitalRead(SW_MASTER1) == LOW) ? 1 : 0);
  Joystick.setButton(3, (digitalRead(SW_MASTER2) == LOW) ? 1 : 0);

  Joystick.setButton(4, (digitalRead(SW_NORM) == LOW) ? 1 : 0);
  Joystick.setButton(5, (digitalRead(SW_CRANK) == LOW) ? 1 : 0);
  Joystick.setButton(6, (digitalRead(SW_IGN) == LOW) ? 1 : 0);

  delay(50);

}


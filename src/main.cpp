// Lab 10 – Event-Driven Programming with Push Buttons and a Touch Sensor
// STARTER FILE – follow the lab procedure and add your code under each comment.
#include <Arduino.h>

// ---------- Pin numbers ----------
#define BUTTON_A   32   // polled button
#define BUTTON_B   33   // interrupt button
#define TOUCH_PIN  27   // touch sensor wire
#define GREEN_LED  25
#define RED_LED    26
#define BUZZER     13

// ---------- Global variables ----------
int lastStateA = HIGH;
int pressCountA = 0;
unsigned long lastChangeA = 0;
const unsigned long DEBOUNCE_TIME = 50;

volatile bool buttonBFlag = false;
volatile unsigned long lastInterruptB = 0;
int pressCountB = 0;

typedef void (*Callback)();
Callback buttonACallback;
Callback buttonBCallback;
Callback touchCallback;

volatile bool touchFlag = false;
volatile unsigned long lastTouch = 0;
const int TOUCH_THRESHOLD = 0.5;

bool armed = false;
bool alarmOn = false;
unsigned long alarmStart = 0;
const unsigned long ALARM_TIME = 3000;

// ---------- Event handlers and callbacks ----------
void onButtonAPressed()
{
  pressCountA++;
  Serial.print("Button A pressed. Count = ");
  Serial.println(pressCountA);
}

void onButtonBPressed()
{
  pressCountB++;
  Serial.print("Button B pressed (interrupt). Count = ");
  Serial.println(pressCountB);
}

// alarmOff() – turns the alarm OFF.
// It is used when the alarm times out, when button B silences it,
// and when the system is disarmed.
void alarmOff()
{
  // 1. Remember that the alarm is no longer on:
  //    set the variable alarmOn to false.
  alarmOn = false;
  // 2. Turn the red LED off.
  //    (Common Commands: "Turn an Output On / Off" – use RED_LED and LOW)
  digitalWrite(RED_LED, LOW);
  // 3. Turn the buzzer off in the same way (use BUZZER and LOW).
  digitalWrite(BUZZER, LOW);
}

// startAlarm() – turns the alarm ON and starts the 3-second timer.
// It is called by onTouchDetected() when the wire is touched while armed.
void startAlarm()
{
  // 1. Remember that the alarm is on: set alarmOn to true.
  alarmOn = true;
  // 2. Start the timer: save the time now in the variable alarmStart.
  //    (Common Commands: "Save the Time Now")
  //    The // Timer code in loop() compares millis() with alarmStart
  //    and turns the alarm off after ALARM_TIME (3000 ms).
  alarmStart = millis();

  // 3. Turn the red LED on (use RED_LED and HIGH).
  digitalWrite(RED_LED, HIGH);
  // 4. Turn the buzzer on (use BUZZER and HIGH).
  digitalWrite(BUZZER, HIGH);
}

// toggleArmed() – the callback for button A.
// Each press switches the system between ARMED and DISARMED.
void toggleArmed()
{
  // 1. Flip the variable armed: true becomes false, false becomes true.
  //    (Common Commands: "Flip a bool")
  armed = !armed;
  // 2. Write an if / else that checks whether armed is true.
  //    If armed is true (the system was just armed):
  //      - turn the green LED on (GREEN_LED, HIGH)
  //      - print  System ARMED  on the Serial Monitor
  //    Else (the system was just disarmed):
  //      - turn the green LED off (GREEN_LED, LOW)
  //      - call alarmOff(), so an alarm that is ringing stops
  //      - print  System DISARMED  on the Serial Monitor
  if (armed == true )
  {
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("System ARMED");
  }
  else
  {
    digitalWrite(GREEN_LED, LOW);
    alarmOff();
    Serial.println("System DISARMED");
  }
}

// silenceAlarm() – the callback for button B.
// It stops the alarm but keeps the system armed.
void silenceAlarm()
{
  // 1. Call alarmOff() to turn the red LED and the buzzer off.
  alarmOff();
  // 2. Print  Alarm silenced  on the Serial Monitor.
  Serial.println("Alarm silenced");
}

// onTouchDetected() – the callback for the touch sensor.
// loop() calls it every time touch_ISR() raises touchFlag.
void onTouchDetected()
{
  // 1. Write an if that checks whether armed is true.
  //    The alarm must only start when the system is armed.
 if (armed == true)
  {
  // 2. Inside the if:
  //      - print  TOUCH! Alarm ON  on the Serial Monitor
  //      - call startAlarm()
  //    If the system is not armed, the function does nothing.
    Serial.println("TOUCH! Alarm ON");
    startAlarm();
  }
}


// ---------- Interrupt Service Routines (ISRs) ----------
void IRAM_ATTR buttonB_ISR()
{
  if (millis() - lastInterruptB > 200)
  {
    lastInterruptB = millis();
    buttonBFlag = true;
  }
}

void IRAM_ATTR touch_ISR()
{
  if (millis() - lastTouch > 1000)
  {
    lastTouch = millis();
    touchFlag = true;
  }
}


void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("Lab 10: Event-Driven Programming");

  // Inputs and outputs
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_B), buttonB_ISR, FALLING);

  attachInterrupt(TOUCH_PIN, touch_ISR, TOUCH_THRESHOLD);
  
  // Callbacks
  buttonACallback = toggleArmed;
  buttonBCallback = silenceAlarm;
  touchCallback = onTouchDetected;

}

void loop()
{
  // Button A (polling)
  int stateA = digitalRead(BUTTON_A);
  if (stateA != lastStateA && millis() - lastChangeA > DEBOUNCE_TIME)
  {
    lastChangeA = millis();
    lastStateA = stateA;
    if (stateA == LOW)
    {
      buttonACallback();
    }
  }

  // Button B (interrupt)
  if (buttonBFlag == true)
  {
    buttonBFlag = false;
    buttonBCallback();
  }

  // Touch sensor (interrupt)
  if (touchFlag == true)
  {
    touchFlag = false;
    touchCallback;
  }

  // Timer
  if (alarmOn == true && millis() - alarmStart >= ALARM_TIME)
  {
    alarmOff();
    Serial.println("Alarm timed out");
  }

}

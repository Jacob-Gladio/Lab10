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


// ---------- Interrupt Service Routines (ISRs) ----------
void IRAM_ATTR buttonB_ISR()
{
  if (millis() - lastInterruptB > 200)
  {
    lastInterruptB = millis();
    buttonBFlag = true;
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

  // Callbacks
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
      onButtonAPressed();
    }
  }

  // Button B (interrupt)
  if (buttonBFlag == true)
  {
    buttonBFlag = false;
    onButtonBPressed();
  }

  // Touch sensor (interrupt)

  // Timer
  Serial.println("Busy...");
  delay(3000);
}

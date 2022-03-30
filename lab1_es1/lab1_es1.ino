#include <TimerOne.h>
const int RLED_PIN = 10;
const int GLED_PIN = 11;

const float R_HALF_PERIOD = 0.25;
const float G_HALF_PERIOD = 0.25;

int greenLedState = LOW;
int redLedState = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);
  Timer1.initialize(G_HALF_PERIOD * 1e06);
  Timer1.attachInterrupt(blinkGreen);
}

void blinkGreen(){
  greenLedState = !greenLedState;
  digitalWrite(GLED_PIN, greenLedState);
  }

void loop() {
  // put your main code here, to run repeatedly:
  redLedState  = !redLedState;
  digitalWrite(RLED_PIN, redLedState);
  delay(R_HALF_PERIOD * 1e03);
}

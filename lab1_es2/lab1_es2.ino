#include <TimerOne.h>
const int RLED_PIN = 10;
const int GLED_PIN = 11;

const float R_HALF_PERIOD = 0.75;
const float G_HALF_PERIOD = 1.5;

int greenLedState = LOW;
int redLedState = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);
  Timer1.initialize(G_HALF_PERIOD * 1e06);
  Timer1.attachInterrupt(blinkGreen);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 1.2 Starting");
  Serial.println("Benvenuto");
  Serial.println("inserire R/G per verificare lo stato dei corrispettivi led");
}

void blinkGreen() {
  greenLedState = !greenLedState;
  digitalWrite(GLED_PIN, greenLedState);
}

void serialPrintStatus() {
  if (Serial.available() > 0) {
    int inByte = Serial.read();
    if (inByte == 'R') {
      Serial.print("Stato led rosso: ");
      Serial.println(redLedState);
    }
    else if (inByte == 'L') {
      Serial.print("Stato led green ");
      Serial.println(greenLedState);
    }
    else {
      //Serial.println("Errore carattere non valido");
      char strBuf[50];
      sprintf(strBuf, "Carattere %c non valido", (char)inByte);
      Serial.println(strBuf);
    }
    Serial.read();
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  redLedState  = !redLedState;
  digitalWrite(RLED_PIN, redLedState);
  delay(R_HALF_PERIOD * 1e03);
  serialPrintStatus();
}

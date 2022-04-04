#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);

const int TEMP_PIN = A1;
const int B = 4275;
const int long R0 = 100000;
const int Vcc = 1023;
const float T0 = 298.15;

void setup() {
  // put your setup code here, to run once:
  pinMode(TEMP_PIN, INPUT);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 1.6 Starting");

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
}

void convertTension (int sensorVal) {
  float R = ((Vcc / sensorVal) - 1.0) * R0;
  float T = 1 / (log(R / R0) / B + 1 / T0) - 273.15;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.print(T);
  delay(1e3);
}

void loop() {
  int sensorVal = analogRead(TEMP_PIN);
  convertTension(sensorVal);
}

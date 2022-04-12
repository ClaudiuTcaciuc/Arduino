#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);
//microphone
const int MIC_PIN = A0; //vedere se funziona provare 4
int n_sound_events = 0;
unsigned long last_event_time = 0;
const long sound_interval = 10l * 60l * 1000l;
//fan
const int FAN_PIN = 6;
//led
const int RLED_PIN = 11;
//sensore di temperatura
const int TEMP_PIN = A1;
//sensore di movimento
const int PIR_PIN = 7;
unsigned long elapsed_time = 0;
const long timeout_pir = 30l * 60l * 1000l;
struct TempPair {
  float low;
  float high;
};
TempPair ct{.low = 25.0, .high = 30.0};
TempPair ht{.low = 17.0, .high = 24.0};

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(MIC_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(RLED_PIN, OUTPUT);

  analogWrite(FAN_PIN, 0);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), checkPresencePIR, CHANGE);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 2 Starting");
  Serial.println("Write 'HTH/ HTL/ CTH/ CTL'");
  Serial.println("followed by a space and a number to change set point");
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();
  lcd.clear();
}

float convertTension(float sensorVal) {
  const int B = 4275;
  const int long R0 = 100000;
  const int Vcc = 1023;
  const float T0 = 298.15;
  const float TBase = 273.15;
  float R = ((Vcc / sensorVal) - 1.0) * R0;
  float T = 1 / (log(R / R0) / B + 1 / T0) - TBase;
  return T;
}

int calcSpeed(float temp) {
  if (temp <= ct.high) {
    float dt = (temp - ct.low) * (255 / (ct.high - ct.low));
    return (int)dt;
  } else {
    return 255;
  }
}

int calcLedIntensity(float temp) {
  if (temp >= ht.low) {
    float dt = (ht.high - temp) * (255 / (ht.high - ht.low));
    return (int)(dt);
  } else {
    return 255;
  }
}

void checkTempAndChangeSpeed(float temp, int pres) {
  int newSpeed = 0;
  int newLedIntensity = 0;
  if (temp >= ct.low) {
    analogWrite(RLED_PIN, LOW);
    newSpeed = calcSpeed(temp);
    if (newSpeed <= 255 && newSpeed >= 0) {
      analogWrite(FAN_PIN, newSpeed);
      Serial.print("New fan speed: ");
      Serial.println(newSpeed);
    }
  }
  else if (temp < ht.high) {
    analogWrite(FAN_PIN, 0);
    newLedIntensity = calcLedIntensity(temp);
    if (newLedIntensity <= 255 && newLedIntensity >= 0) {
      analogWrite(RLED_PIN, newLedIntensity);
      Serial.print("New led intensity: ");
      Serial.println(newLedIntensity);
    }
  }
  else {
    analogWrite(RLED_PIN, LOW);
    analogWrite(FAN_PIN, 0);
  }
  char buf[10] {};
  Serial.println(buf);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print(" Pres:");
  lcd.print(pres);
  lcd.setCursor(0, 1);
  lcd.print("AC:");
  lcd.print(100 * newSpeed / 255);
  lcd.print("%");
  lcd.print(" HT:");
  lcd.print(100 * newLedIntensity / 255);
  lcd.print("%");
  delay(5e3);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AC m:");
  dtostrf(ct.low, 2, 1, buf);
  lcd.print(buf);
  lcd.print(" M:");
  dtostrf(ct.high, 2, 1, buf);
  lcd.print(buf);
  lcd.setCursor(0, 1);
  lcd.print("HT m:");
  dtostrf(ht.low, 2, 1, buf);
  lcd.print(buf);
  lcd.print(" M:");
  dtostrf(ht.high, 2, 1, buf);
  lcd.print(buf);
  delay(5e3);
}

void checkPresencePIR() {
  if ( digitalRead(PIR_PIN) == HIGH )
    elapsed_time = millis();
}

void checkPresenceMIC() {
  if (digitalRead(MIC_PIN) == LOW) {
    last_event_time = millis();
    if (millis() - last_event_time > 25)
      n_sound_events++;
  }
}

bool checkPresenceRoom() {
  bool check_mic = true, check_pir = true;
  checkPresenceMIC();
  if (n_sound_events <= 50 && last_event_time + sound_interval <= millis()) {
    Serial.println("No detection for over 10 mins with sound sensor");
    n_sound_events = 0;
    check_mic = false;
  }
  if (elapsed_time + timeout_pir <= millis()) {
    Serial.println("No detection for over 30 mins with pir sensor");
    check_mic = false;
  }
  if (check_mic == true || check_pir == true)
    return true;
  return false;
}

float readSetPoint() {
  while (Serial.available() == 0) {};
  return Serial.parseFloat();
}

void setDefaultSetPoint() {
  ct.low = 25.0; ct.high = 30.0;
  ht.low = 17.0; ht.high = 24.0;
}

void setNoOneSetPoint() {
  ct.low = 30.0; ct.high = 40.0;
  ht.low = 10.0; ht.high = 18.0;
}

void updateFormatError() {
  Serial.println("Error in command format");
  return;
}

void checkCharAndUpdateVal(char c, TempPair& temp, float value) {
  Serial.println(value);
  switch (c) {
    case 'L':
      temp.low = value;
      break;
    case 'H':
      temp.high = value;
      break;
    default:
      return updateFormatError();
  }
}

/*
  "HTLxxx" => ht.low
  "HTHxxx" => ht.high
  "CTLxxx" => ct.low
  "CTHxxx" => ct.high
*/
void checkAndChangeSetPoint() {
  if (Serial.available() > 0) {
    auto command = Serial.readString();
    if (command.length() < 5) {
      return updateFormatError();
    }
    if (command[3] != ' ')
      return updateFormatError();
    float value = command.substring(4).toFloat();
    if (command.startsWith("HT")) {
      return checkCharAndUpdateVal(command[2], ht, value);
    } else if (command.startsWith("CT")) {
      return checkCharAndUpdateVal(command[2], ct, value);
    } else {
      return updateFormatError();
    }
  }
}

void setTempBasedOnPresence(float temp) {
  int pres = 0;
  if (checkPresenceRoom() == true) {
    Serial.println("There is at least one person is this room");
    pres = 1;
  }
  else {
    setNoOneSetPoint();
    pres = 0;
  }
  checkTempAndChangeSpeed(temp, pres);
}

void loop() {
  float sensorVal = analogRead(TEMP_PIN);
  float temp = convertTension(sensorVal);
  Serial.print("Temperatura: ");
  Serial.println(temp);
  checkAndChangeSetPoint();
  setTempBasedOnPresence(temp);
}

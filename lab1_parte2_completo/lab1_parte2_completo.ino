#include <LiquidCrystal_PCF8574.h>
LiquidCrystal_PCF8574 lcd(0x27);
//microphone
const int MIC_PIN = A0;
int n_sound_events = 0;
unsigned long last_event_time = 0;
const long sound_interval = 10l * 60l * 1000l;
//fan
const int FAN_PIN = 6;
//led
const int RLED_PIN = 11;
const int GLED_PIN = 10;
//sensore di temperatura
const int TEMP_PIN = A1;
//sensore di movimento
const int PIR_PIN = 7;
unsigned long elapsed_time = 0;
const long timeout_pir = 30l * 60l * 1000l;
const int display1_period = 5000;
const int display2_period = 10000;
unsigned long previous_time_display1 = 0;
unsigned long previous_time_display2 = 0;
int flag_changed = 0;
int clap = 0;
struct TempPair {
  float low;
  float high;
};

TempPair ct_noOne{.low = 30.0, .high = 40.0};
TempPair ht_noOne{.low = 10.0, .high = 15.0};

TempPair ct_default{.low = 25.0, .high = 30.0};
TempPair ht_default{.low = 17.0, .high = 24.0};

TempPair ct_changed = ct_default;
TempPair ht_changed = ht_default;

TempPair ct = ct_default;
TempPair ht = ht_default;

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(MIC_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);

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
  } else
    return 255;
}

int calcLedIntensity(float temp) {
  if (temp >= ht.low) {
    float dt = (ht.high - temp) * (255 / (ht.high - ht.low));
    return (int)(dt);
  } else
    return 255;
}

void setDefaultSetPoint() {
  ct = ct_default;
  ht = ht_default;
}

void setNoOneSetPoint() {
  ct = ct_noOne;
  ht = ht_noOne;
}

void setChangedSetPoint() {
  ct = ct_changed;
  ht = ht_changed;
}

void checkTempAndChangeSpeed(float temp, bool pres, unsigned long time_point) {
  int newSpeed = 0;
  int newLedIntensity = 0;
  if (!pres)
    setNoOneSetPoint();
  else if (flag_changed == 0)
    setDefaultSetPoint();
  else
    setChangedSetPoint();

  if (temp >= ct.low) {
    analogWrite(RLED_PIN, LOW);
    newSpeed = calcSpeed(temp);
    if (newSpeed <= 255 && newSpeed >= 0)
      analogWrite(FAN_PIN, newSpeed);
  }
  else if (temp < ht.high) {
    analogWrite(FAN_PIN, 0);
    newLedIntensity = calcLedIntensity(temp);
    if (newLedIntensity <= 255 && newLedIntensity >= 0)
      analogWrite(RLED_PIN, newLedIntensity);
  }
  else {
    analogWrite(RLED_PIN, LOW);
    analogWrite(FAN_PIN, 0);
  }
  String bufPage1, bufPage2;
  if (time_point >= previous_time_display1 + display1_period) {
    lcd.clear();
    lcd.setCursor(0, 0);
    bufPage1 = String("AC m:") + String(ct.low, 1) + " M:" + String(ct.high, 1);
    bufPage2 = String("HT m:") + String(ht.low, 1) + " M:" + String(ht.high, 1);
    lcd.print(bufPage1);
    lcd.setCursor(0, 1);
    lcd.print(bufPage2);
    previous_time_display1 += display1_period;
  }
  if (time_point >= previous_time_display2 + display2_period) {
    lcd.clear();
    lcd.setCursor(0, 0);
    bufPage1 = String("T:") + String(temp) + " Pres:" + (pres ? "si" : "no");
    bufPage2 = String("AC:") + String(100 * newSpeed / 255) + "% HT:" + String(100 * newLedIntensity / 255) + "%";
    lcd.print(bufPage1);
    lcd.setCursor(0, 1);
    lcd.print(bufPage2);
    previous_time_display2 += display2_period;
  }
}

void checkPresencePIR() {
  if ( digitalRead(PIR_PIN) == HIGH )
    elapsed_time = millis();
}

/* Richiesta 4
  void checkPresenceMIC(unsigned long time_point) {
  if (digitalRead(MIC_PIN) == LOW)
    if (time_point - last_event_time > 25) {
      n_sound_events++;
      last_event_time = time_point;
    }
  }
*/
bool checkPresenceRoom(unsigned long time_point) {
  bool check_mic = true, check_pir = true;
  /*Richiesta 4
    checkPresenceMIC(time_point);
    if (n_sound_events <= 50 && last_event_time + sound_interval <= time_point) {
    Serial.println("No detection for over 10 mins with sound sensor");
    n_sound_events = 0;
    check_mic = false;
    }
  */
  if (elapsed_time + timeout_pir <= time_point) {
    Serial.println("No detection for over 30 mins with pir sensor");
    check_pir = false;
  }
  if (check_mic || check_pir)
    return true;
  return false;
}

float readSetPoint() {
  while (Serial.available() == 0) {};
  return Serial.parseFloat();
}

void updateFormatError() {
  Serial.println("Error in command format or value is not acceptable");
  return;
}

void checkCharAndUpdateVal(char c, TempPair & temp, float value) {
  switch (c) {
    case 'L':
      if (value > temp.high)
        return updateFormatError();
      temp.low = value;
      break;
    case 'H':
      if (value < temp.low)
        return updateFormatError();
      temp.high = value;
      break;
    default:
      return updateFormatError();
  }
}

/*
  "HTL xx" => ht.low
  "HTH xx" => ht.high
  "CTL xx" => ct.low
  "CTH xx" => ct.high
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
      if (value > ct_changed.low)
        return updateFormatError();
      flag_changed = 1;
      return checkCharAndUpdateVal(command[2], ht_changed, value);
    } else if (command.startsWith("CT")) {
      if (value < ht_changed.high)
        return updateFormatError();
      flag_changed = 1;
      return checkCharAndUpdateVal(command[2], ct_changed, value);
    } else
      return updateFormatError();
  }
}

void setTempBasedOnPresence(float temp, unsigned long time_point) {
  checkTempAndChangeSpeed(temp, checkPresenceRoom(time_point), time_point);
}

//richiesta 9
void turnOnOffLight(int stat_light) {
  digitalWrite(GLED_PIN, stat_light);
  clap = 0;
}

void checkSoundLightActivation(unsigned long time_point) {
  if (digitalRead(MIC_PIN) == LOW ) {
    if (time_point - last_event_time < 500 && clap > 0) 
      clap++;
    else if (clap == 0) {
      last_event_time = time_point;
      clap++;
    }
    else
      clap = 0;
    if (digitalRead(GLED_PIN) == LOW && clap >= 2)
      turnOnOffLight(HIGH);
    else if (digitalRead(GLED_PIN) == HIGH && clap >= 2)
      turnOnOffLight(LOW);
  }
}

void loop() {
  unsigned long time_point = millis();
  checkAndChangeSetPoint();
  float sensorVal = analogRead(TEMP_PIN);
  float temp = convertTension(sensorVal);
  setTempBasedOnPresence(temp, time_point);
  checkSoundLightActivation(time_point);
  delay(1e1);
}

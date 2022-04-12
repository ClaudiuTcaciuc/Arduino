//microphone
const int MIC_PIN = A0; //vedere se funziona provare 4
int n_sound_events = 0;
unsigned long last_event_time = 0;
const long sound_interval = 10l * 60l * 1000l;
//sensore fan
const int FAN_PIN = 6;
//sensore led
const int RLED_PIN = 11;
//sensore di temperatura
const int TEMP_PIN = A1;
//sensore di movimento
const int PIR_PIN = 7;
unsigned long elapsed_time = 0;
const long timeout_pir = 30l * 60l * 1000l;

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

int calcSpeed(float temp, float ct_high, float ct_low) {
  if (temp <= ct_high) {
    float dt = (temp - ct_low) * (255 / (ct_high - ct_low));
    return (int)dt;
  } else {
    return 255;
  }
}

int calcLedIntensity(float temp, float ht_high, float ht_low) {
  if (temp >= ht_low) {
    float dt = (ht_high - temp) * (255 / (ht_high - ht_low));
    return (int)(dt);
  } else {
    return 255;
  }
}

void checkTempAndChangeSpeed(float temp, float ht_low, float ht_high, float ct_low, float ct_high) {
  if (temp >= ct_low) {
    analogWrite(RLED_PIN, LOW);
    int newSpeed = calcSpeed(temp, ct_high, ct_low);
    if (newSpeed <= 255 && newSpeed >= 0) {
      analogWrite(FAN_PIN, newSpeed);
      Serial.print("New fan speed: ");
      Serial.println(newSpeed);
    }
  }
  else if (temp < ht_high) {
    analogWrite(FAN_PIN, 0);
    int newLedIntensity = calcLedIntensity(temp, ht_high, ht_low);
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

void checkAndChangeSetPoint(float temp) {
  float ht_high = 20, ht_low = 10, ct_high = 40, ct_low = 30;
  if (checkPresenceRoom() == true) {
    ht_high = 25.0; ht_low = 20.0; ct_high = 30.0; ct_low = 25.0;
    Serial.println("There is at least one person is this room");
    checkTempAndChangeSpeed(temp, ht_low, ht_high, ct_low, ct_high);
  }
  checkTempAndChangeSpeed(temp, ht_low, ht_high, ct_low, ct_high);
}

void loop() {
  float sensorVal = analogRead(TEMP_PIN);
  float temp = convertTension(sensorVal);
  Serial.print("Temperatura: ");
  Serial.println(temp);
  checkAndChangeSetPoint(temp);
  delay(1e4);
}

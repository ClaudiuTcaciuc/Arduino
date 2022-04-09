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

void setup() {
  pinMode(TEMP_PIN, INPUT);
  pinMode(RLED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  analogWrite(FAN_PIN, 0);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), checkPresence, CHANGE);
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

int calcSpeed(float temp) {
  if (temp <= 38.0) {
    float dt = (temp - 25.0) * 20.0;
    return (int)dt;
  } else {
    return 255;
  }
}

int calcLedIntensity(float temp) {
  if (temp >= 20.0) {
    float dt = (25.0 - temp) * 51.0;
    return (int)(dt);
  } else {
    return 255;
  }
}

void checkTempAndChangeSpeed(float temp) {
  if (temp >= 25.0) {
    analogWrite(RLED_PIN, LOW);
    int newSpeed = calcSpeed(temp);
    if (newSpeed <= 255 && newSpeed >= 0)
      analogWrite(FAN_PIN, newSpeed);
  }
  else {
    analogWrite(FAN_PIN, 0);
    int newLedIntensity = calcLedIntensity(temp);
    if (newLedIntensity <= 255 && newLedIntensity >= 0)
      analogWrite(RLED_PIN, newLedIntensity);
  }
}

void checkPresence() {
  if ( digitalRead(PIR_PIN) == HIGH )
    elapsed_time = millis();
}

void loop() {
  float sensorVal = analogRead(TEMP_PIN);
  float temp = convertTension(sensorVal);
  Serial.print("Temperatura: ");
  Serial.println(temp);
  checkTempAndChangeSpeed(temp);
  if (elapsed_time + timeout_pir <= millis()) {
    Serial.println("No detection for over 30 mins");
  }
  delay(1e4);
}

const int TEMP_PIN = A1;
const int LED_PIN = 11;
const int FAN_PIN = 6;
const int B = 4275;
const int long R0 = 100000;
const int Vcc = 1023;
const float T0 = 298.15;
void setup() {
  pinMode(TEMP_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 1 parte 2 starting");
}
float convertTension () {
  float sensorVal = analogRead(TEMP_PIN);
  float R = ((Vcc / sensorVal) - 1.0) * R0;
  float T = 1 / (log(R / R0) / B + 1 / T0) - 273.15;
  Serial.print("Temperatura: ");
  Serial.println(T);
  return T;
}
void changeSpeed() {
  float T = convertTension();
  // float T = convertTension(sensorVal);
  int V;
  if (T >= 25 && T <= 38) {
    V = (T - 25) * 20;
  }
  else if (T > 38) {
    V = 255;
  }
  else {
    V = 0;
  }
  analogWrite(FAN_PIN, V);
  Serial.print("velocità ventola: ");
  Serial.println(V);
  delay(1e4);
}
void changeLed() {
  float T = convertTension();
  int I;
  if (T <= 26 && T >= 13) {
    I = (26 - T) * 20;
  }
  else if (T < 13) {
    I = 255;
  }
  else {
    I = 0;
  }
  analogWrite(LED_PIN, I);
  Serial.print("intensità led: ");
  Serial.println(I);
}

void loop() {
  changeSpeed();
  changeLed();
}

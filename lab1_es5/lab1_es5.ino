
const int TEMP_PIN = A1;
const int B = 4275;
const int long R0 = 100000;
const int Vcc = 1023;
const float T0 = 298;
void setup() {
  // put your setup code here, to run once:
  pinMode(TEMP_PIN, INPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 1.5 Starting");
}

void convertTension (int sensorVal) {
  float Vsig = (sensorVal / 1024.0) * 5.0;
  float R = ((Vcc / Vsig) - 1) * R0;
  float T = 1 / ((log(R / R0) / B) + (1 / T0));
  float TCelsius = T - 273.15;
  Serial.print("Temperatura: ");
  Serial.println(TCelsius);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorVal = analogRead(TEMP_PIN);
  convertTension (sensorVal);
  delay(1e3);
}

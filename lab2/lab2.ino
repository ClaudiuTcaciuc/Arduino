//fan
const int FAN_PIN = 6;
//led
const int RLED_PIN = 11;
float currentSpeed = 0;
//sensore di temperatura
const int TEMP_PIN = A1;
const int B = 4275;
const int long R0 = 100000;
const int Vcc = 1023;
const float T0 = 298.15;

void setup() {
  pinMode(TEMP_PIN, INPUT);
  pinMode(RLED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, (int) currentSpeed);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 2 Starting");
}

float convertTension (float sensorVal) {
  float R = ((Vcc / sensorVal) - 1.0) * R0;
  float T = 1 / (log(R / R0) / B + 1 / T0) - 273.15;
  Serial.print("Temperatura: ");
  Serial.println(T);
  return T;
}

int SetSpeed (float Temp) {
  if (Temp >= 25) {
    if (Temp <= 38) {
      float DT = (Temp - 25) * 20; return (int)DT;
    } else {
      return 255;
    }
  } else {
    return 0;
  }
}

int SetLed(float Temp) {
  if (Temp < 25) {
    if (Temp >= 20) {
      float DT = (Temp - 20) * 51; return (int)DT;
    } else {
      return 255;
    }
  } else {
    return 0;
  }
}

void change(int Temp) {
  if (Temp >= 25) {
    analogWrite(RLED_PIN, LOW)
    int newSpeed = SetSpeed(Temp);
    changeSpeed(newSpeed);
  }
  else {
    changeSpeed(0);
    int newLed = SetLed(Temp);
    analogWrite(RLED_PIN, newLed);
  }
}

void changeSpeed(int new_speed) {
  if (new_speed <= 255 && new_speed >= 0) {
    currentSpeed = new_speed;
    analogWrite(FAN_PIN, (int) currentSpeed);
  }
}

void loop() {
  float sensorVal = analogRead(TEMP_PIN);
  float Temp = convertTension (sensorVal);
  change(Temp);
  delay(1e4);
}

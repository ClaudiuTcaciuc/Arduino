
const int FAN_PIN = 6;
float current_speed = 0;

void setup() {
  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, (int) current_speed);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 1.4 Starting");
}

void changeSpeed(int change) {
  int new_speed = current_speed + change;
  if (new_speed <= 255 && new_speed >= 0) {
    current_speed = new_speed;
    analogWrite(FAN_PIN, (int) current_speed);
    Serial.print("Speed: ");
    Serial.println(current_speed);
  }
  else {
    Serial.println("speed limit reached");
  }
}

void increaseSpeed() {
  if (Serial.available() > 0) {
    int inByte = Serial.read();
    if (inByte == '+') {
      changeSpeed(10);
    }
    else if (inByte == '-') {
      changeSpeed(-10);
    }
    else if (inByte == '0') {
      changeSpeed(-current_speed);
    }
    else {
      Serial.println("Invalid input, insert +/- or 0");
    }
    Serial.read(); // legge lo \n
  }
}

void loop() {
  increaseSpeed();
}

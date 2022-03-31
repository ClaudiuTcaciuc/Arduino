const int GLED_PIN = 10;
const int PIR_PIN = 7;

volatile int tot_count = 0;
int greenLedState = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(GLED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), checkPresence, CHANGE);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Lab 1.3 Starting");
}

void checkPresence() {
  int motion = digitalRead(PIR_PIN);
  if ( motion == HIGH ) {
    tot_count++;
    digitalWrite(GLED_PIN, HIGH);
    delay(3e3);
  }
  else {
    digitalWrite(GLED_PIN, LOW);
    delay(3e3);
  }
}

void loop() {
  delay(3e4);
  Serial.print("Total people count: ");
  Serial.println(tot_count);
}

/*
  ==========================================================
   AUTOMATIC DOOR CONTROL SYSTEM (PIR Sensor Based)
  ==========================================================
   Program Flow (as per flowchart):
   1. Start
   2. Declare variables Motor_pin1, Motor_pin2
   3. Compare the incoming value from PIR Sensor
   4. If Motor_pin1 = HIGH and Motor_pin2 = LOW  -> Door CLOSES
   5. If Motor_pin1 = LOW  and Motor_pin2 = HIGH -> Door OPENS
  ==========================================================
*/

// ---------- Pin Declarations ----------
const int PIR_sensor = 2;     // PIR sensor OUT pin connected here
const int Motor_pin1 = 6;     // Motor driver input 1 (e.g., IN1 of L293D/L298N)
const int Motor_pin2 = 7;     // Motor driver input 2 (e.g., IN2 of L293D/L298N)

int pirValue = 0;              // Stores incoming value from PIR sensor
const unsigned long doorOpenTime = 5000;  // Time (ms) door stays open once triggered

void setup() {
  Serial.begin(9600);

  // Configure PIR sensor as input
  pinMode(PIR_sensor, INPUT);

  // Configure motor pins as output
  pinMode(Motor_pin1, OUTPUT);
  pinMode(Motor_pin2, OUTPUT);

  // Ensure door starts in the CLOSED state
  digitalWrite(Motor_pin1, HIGH);
  digitalWrite(Motor_pin2, LOW);

  Serial.println("System Initialized. Door is CLOSED.");
}

void loop() {
  // Step 3: Compare the incoming value from PIR Sensor
  pirValue = digitalRead(PIR_sensor);

  if (pirValue == HIGH) {
    // Motion detected -> OPEN the door
    // Condition: Motor_pin1 = LOW, Motor_pin2 = HIGH
    openDoor();

    // Keep the door open for a fixed duration
    delay(doorOpenTime);

    // Re-check PIR after wait; if no more motion, close the door
    pirValue = digitalRead(PIR_sensor);
    if (pirValue == LOW) {
      closeDoor();
    }
  } else {
    // No motion detected -> keep/CLOSE the door
    // Condition: Motor_pin1 = HIGH, Motor_pin2 = LOW
    closeDoor();
  }

  delay(200); // Small debounce delay before next sensor check
}

// ---------- Function: Open Door ----------
void openDoor() {
  digitalWrite(Motor_pin1, LOW);
  digitalWrite(Motor_pin2, HIGH);
  Serial.println("Motion Detected -> Door OPENING");
}

// ---------- Function: Close Door ----------
void closeDoor() {
  digitalWrite(Motor_pin1, HIGH);
  digitalWrite(Motor_pin2, LOW);
  Serial.println("No Motion -> Door CLOSED");
}

/*
  ==========================================================
   AUTOMATIC GUIDED VEHICLE (AGV)
   (IR Sensor + Ultrasonic Sensor based Direction Control)
   Based on DAMS - UCT / UpSkill Campus IoT Product
  ==========================================================

   Program Flow (as per flowchart):
   1. Start
   2. Declare Variables: Motor_pin1, Motor_pin2,
      sensor_value1, sensor_value2
   3. Read the data from IR sensor and Ultrasonic sensor
   4. With the help of sensors:
        - If path is clear -> Motor_pin1 = HIGH, Motor_pin2 = LOW
          -> Robot direction: FORWARD
        - If obstacle detected -> Motor_pin1 = LOW, Motor_pin2 = HIGH
          -> Robot direction: BACKWARD

   Hardware Used:
   - Arduino UNO
   - IR Sensor        -> Digital pin 3   (sensor_value1)
   - Ultrasonic Sensor (HC-SR04):
        TRIG -> pin 9
        ECHO -> pin 10                  (sensor_value2, distance based)
   - Motor Driver (L293D/L298N):
        Motor_pin1 -> pin 6
        Motor_pin2 -> pin 7
  ==========================================================
*/

// ---------- Pin Declarations ----------
const int IR_sensor   = 3;    // IR sensor digital output
const int TRIG_PIN    = 9;    // Ultrasonic sensor trigger
const int ECHO_PIN    = 10;   // Ultrasonic sensor echo

const int Motor_pin1  = 6;    // Motor driver input 1
const int Motor_pin2  = 7;    // Motor driver input 2

// ---------- Variable Declarations ----------
int sensor_value1 = 0;        // Stores IR sensor reading
long sensor_value2 = 0;       // Stores ultrasonic distance (cm)

const int obstacleDistance = 15;  // Distance threshold (cm) to detect obstacle

void setup() {
  Serial.begin(9600);

  pinMode(IR_sensor, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(Motor_pin1, OUTPUT);
  pinMode(Motor_pin2, OUTPUT);

  // Start in FORWARD direction
  digitalWrite(Motor_pin1, HIGH);
  digitalWrite(Motor_pin2, LOW);

  Serial.println("AGV System Initialized. Moving FORWARD.");
}

void loop() {
  // Step 3: Read the data from IR sensor and Ultrasonic sensor
  sensor_value1 = digitalRead(IR_sensor);
  sensor_value2 = readUltrasonicDistance();

  Serial.print("IR Sensor Value: ");
  Serial.print(sensor_value1);
  Serial.print("  |  Ultrasonic Distance (cm): ");
  Serial.println(sensor_value2);

  // Step 4: Decide direction based on sensor readings
  if (sensor_value1 == HIGH && sensor_value2 > obstacleDistance) {
    // Path is clear -> Move FORWARD
    moveForward();
  } else {
    // Obstacle detected either by IR or Ultrasonic -> Move BACKWARD
    moveBackward();
  }

  delay(200); // Small delay before next sensor check
}

// ---------- Function: Read Distance from Ultrasonic Sensor ----------
long readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms (~5m range)
  long distance = duration * 0.034 / 2;           // Convert to cm

  if (duration == 0) {
    distance = 999; // No echo received, treat as "clear path"
  }

  return distance;
}

// ---------- Function: Move Forward ----------
void moveForward() {
  digitalWrite(Motor_pin1, HIGH);
  digitalWrite(Motor_pin2, LOW);
  Serial.println(">> Path Clear -> Robot direction: FORWARD");
}

// ---------- Function: Move Backward ----------
void moveBackward() {
  digitalWrite(Motor_pin1, LOW);
  digitalWrite(Motor_pin2, HIGH);
  Serial.println(">> Obstacle Detected -> Robot direction: BACKWARD");
}

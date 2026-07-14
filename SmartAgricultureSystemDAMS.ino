/*
  ==========================================================
   SMART AGRICULTURE SYSTEM
   (Based on DAMS - Data Acquisition and Monitoring System,
    a UCT / UpSkill Campus IoT Product)
  ==========================================================

   Description:
   This system automates irrigation and monitors field
   conditions using a soil moisture sensor and a DHT11
   temperature & humidity sensor. Based on sensor readings,
   the water pump (via relay module) is automatically
   switched ON/OFF, and all data is logged to the Serial
   Monitor (can be extended to send data to DAMS cloud
   dashboard via ESP8266/ESP32).

   Components Used:
   - Arduino UNO
   - Soil Moisture Sensor      -> Analog pin A0
   - DHT11 Temperature/Humidity Sensor -> Digital pin 2
   - Relay Module (Water Pump) -> Digital pin 8
   - LDR (Light Sensor)        -> Analog pin A1  (optional)
   - LED (status indicator)    -> Digital pin 13

   Note: Requires the "DHT sensor library" by Adafruit.
   Install via: Sketch -> Include Library -> Manage Libraries -> search "DHT sensor library"
  ==========================================================
*/

#include <DHT.h>

// ---------- Pin Declarations ----------
#define SOIL_MOISTURE_PIN   A0
#define LDR_PIN             A1
#define DHT_PIN             2
#define RELAY_PIN           8     // Controls water pump
#define STATUS_LED          13

#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// ---------- Threshold Values (tune to your soil/crop) ----------
const int SOIL_DRY_THRESHOLD   = 500;   // Below this = dry soil -> pump ON
const int SOIL_WET_THRESHOLD   = 700;   // Above this = wet soil -> pump OFF
const int LIGHT_LOW_THRESHOLD  = 300;   // Optional: low light detection

// ---------- Variables ----------
int soilMoistureValue = 0;
int lightValue = 0;
float temperature = 0.0;
float humidity = 0.0;
bool pumpStatus = false;

unsigned long lastReadTime = 0;
const unsigned long readInterval = 2000; // Read sensors every 2 seconds

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);   // Pump OFF initially (adjust HIGH/LOW to your relay module type)
  digitalWrite(STATUS_LED, LOW);

  dht.begin();

  Serial.println("==================================================");
  Serial.println(" Smart Agriculture System (DAMS - UCT Product)");
  Serial.println(" System Initialized...");
  Serial.println("==================================================");
}

void loop() {
  if (millis() - lastReadTime >= readInterval) {
    lastReadTime = millis();
    readSensors();
    controlIrrigation();
    displayStatus();
  }
}

// ---------- Function: Read All Sensors ----------
void readSensors() {
  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  lightValue = analogRead(LDR_PIN);
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if DHT read failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Warning: Failed to read from DHT sensor!");
    humidity = -1;
    temperature = -1;
  }
}

// ---------- Function: Control Irrigation Pump ----------
void controlIrrigation() {
  if (soilMoistureValue < SOIL_DRY_THRESHOLD && !pumpStatus) {
    // Soil is dry -> turn pump ON
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(STATUS_LED, HIGH);
    pumpStatus = true;
    Serial.println(">> Soil is DRY -> Water Pump turned ON");
  }
  else if (soilMoistureValue > SOIL_WET_THRESHOLD && pumpStatus) {
    // Soil is sufficiently wet -> turn pump OFF
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(STATUS_LED, LOW);
    pumpStatus = false;
    Serial.println(">> Soil is WET -> Water Pump turned OFF");
  }
}

// ---------- Function: Display Sensor Data & Status ----------
void displayStatus() {
  Serial.println("--------------------------------------------------");
  Serial.print("Soil Moisture Value : ");
  Serial.println(soilMoistureValue);

  Serial.print("Light Level (LDR)   : ");
  Serial.println(lightValue);

  Serial.print("Temperature (C)     : ");
  Serial.println(temperature);

  Serial.print("Humidity (%)        : ");
  Serial.println(humidity);

  Serial.print("Water Pump Status   : ");
  Serial.println(pumpStatus ? "ON" : "OFF");
  Serial.println("--------------------------------------------------");
}

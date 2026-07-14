/*
  ==========================================================
   AIR QUALITY MONITORING SYSTEM
   (MQ135 Gas Sensor + MQTT + OLED Display - SSD1306)
   Based on DAMS - UCT / UpSkill Campus IoT Product
  ==========================================================

   Program Flow (as per flowchart):
   1. Start
   2. Declare Variable Sensor_value
   3. Read the data from MQ135 and publish it using MQTT
   4. We can see the data on OLED (SSD1306)
   5. We can also see the data on MQTT Server

   Hardware Used:
   - ESP8266 / ESP32 (Wi-Fi + MQTT capability required)
   - MQ135 Air Quality Sensor -> Analog pin (A0 on ESP8266)
   - OLED Display SSD1306 (I2C) -> SDA/SCL pins

   Required Libraries (Install via Library Manager):
   - Adafruit SSD1306
   - Adafruit GFX Library
   - PubSubClient (for MQTT)
   - ESP8266WiFi (built-in, if using ESP8266)  OR  WiFi.h (if using ESP32)
  ==========================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

// ---------- OLED Display Settings ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Wi-Fi Credentials ----------
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ---------- MQTT Broker Settings ----------
const char* mqtt_server   = "broker.hivemq.com";   // Replace with your MQTT broker (e.g., DAMS cloud broker)
const int   mqtt_port     = 1883;
const char* mqtt_topic    = "dams/airquality";
const char* mqtt_clientID = "ESP_AirQuality_Client";

WiFiClient espClient;
PubSubClient client(espClient);

// ---------- Pin Declaration ----------
#define MQ135_PIN A0     // MQ135 analog output pin

// ---------- Variable Declaration ----------
int Sensor_value = 0;

unsigned long lastMsgTime = 0;
const unsigned long publishInterval = 3000; // publish every 3 seconds

void setup() {
  Serial.begin(115200);

  pinMode(MQ135_PIN, INPUT);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Air Quality Monitor");
  display.println("Initializing...");
  display.display();

  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  if (millis() - lastMsgTime >= publishInterval) {
    lastMsgTime = millis();

    // Step: Read the data from MQ135
    Sensor_value = analogRead(MQ135_PIN);

    // Publish the data using MQTT
    publishSensorData();

    // Show the data on OLED (SSD1306)
    updateOLED();
  }
}

// ---------- Function: Connect to Wi-Fi ----------
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ---------- Function: Reconnect to MQTT Broker ----------
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_clientID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 3 seconds");
      delay(3000);
    }
  }
}

// ---------- Function: Publish Sensor Data via MQTT ----------
void publishSensorData() {
  char msg[20];
  snprintf(msg, sizeof(msg), "%d", Sensor_value);

  client.publish(mqtt_topic, msg);

  Serial.print("Published Sensor_value: ");
  Serial.println(msg);
}

// ---------- Function: Display Data on OLED ----------
void updateOLED() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Air Quality Monitor");
  display.println("--------------------");
  display.print("MQ135 Value: ");
  display.println(Sensor_value);

  display.setCursor(0, 40);
  if (Sensor_value < 300) {
    display.println("Air Quality: GOOD");
  } else if (Sensor_value < 600) {
    display.println("Air Quality: MODERATE");
  } else {
    display.println("Air Quality: POOR!");
  }

  display.display();
}

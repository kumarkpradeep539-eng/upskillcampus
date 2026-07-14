#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Pin & Object Setup ----------
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- WiFi & MQTT Setup (Tinkercad simulated network) ----------
const char* ssid = "Tinkercad";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com"; // Tinkercad's default simulated broker
WiFiClient espClient;
PubSubClient client(espClient);

// ---------- Declare Variables ----------
float Temp = 0.0;
float Hum = 0.0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // WiFi connect
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  client.setServer(mqtt_server, 1883);
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("MQTT connected");
    } else {
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // ---- Read the data from DHT11 ----
  Hum = dht.readHumidity();
  Temp = dht.readTemperature();

  if (isnan(Hum) || isnan(Temp)) {
    Serial.println("Failed to read from DHT11 sensor!");
    delay(2000);
    return;
  }

  // ---- Publish it to MQTT Server ----
  char tempStr[8];
  char humStr[8];
  dtostrf(Temp, 4, 2, tempStr);
  dtostrf(Hum, 4, 2, humStr);

  client.publish("home/dht11/temperature", tempStr);
  client.publish("home/dht11/humidity", humStr);

  Serial.print("Published -> Temp: ");
  Serial.print(tempStr);
  Serial.print(" C, Hum: ");
  Serial.print(humStr);
  Serial.println(" %");

  // ---- Show data on OLED (SSD1306) ----
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DHT11 Sensor Data");

  display.setTextSize(2);
  display.setCursor(0, 16);
  display.print("T: ");
  display.print(Temp);
  display.println(" C");

  display.setCursor(0, 40);
  display.print("H: ");
  display.print(Hum);
  display.println(" %");

  display.display();

  delay(3000); // read every 3 seconds
}

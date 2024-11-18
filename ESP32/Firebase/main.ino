#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Firebase credentials
#define API_KEY "AIzaSyAg4rRjy0C5hWDee57u_QnwVD6-w8pMtr0"
#define DATABASE_URL "https://pruebaesp32-8acd2-default-rtdb.firebaseio.com"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Pin definitions
#define WATER_PUMP_PIN 13  // Change to your pump's GPIO pin
#define DHT_PIN 4         // Change to your DHT sensor's pin
#define MOISTURE_PIN 36   // Change to your moisture sensor's pin

void setup() {
  Serial.begin(115200);

  // Initialize pump pin
  pinMode(WATER_PUMP_PIN, OUTPUT);
  digitalWrite(WATER_PUMP_PIN, LOW);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready()) {
    // Read sensor data
    float temperature = readTemperature();
    float humidity = readHumidity();
    float moisture = readMoisture();

    // Update sensor readings in Firebase
    String path = "/UsersData/" + String(auth.token.uid.c_str());

    FirebaseJson json;
    json.set("temperature", temperature);
    json.set("humidity", humidity);
    json.set("moisture", moisture);

    Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json);

    // Check for watering commands
    if (Firebase.RTDB.getString(&fbdo, path + "/commands/water/action")) {
      String action = fbdo.stringData();
      if (action == "START") {
        digitalWrite(WATER_PUMP_PIN, HIGH);
        Serial.println("Watering started");
      } else if (action == "STOP") {
        digitalWrite(WATER_PUMP_PIN, LOW);
        Serial.println("Watering stopped");
      }
    }
  }

  delay(2000);  // Update every 2 seconds
}

float readTemperature() {
  // Implement your temperature sensor reading code here
  return 25.0; // Example value
}

float readHumidity() {
  // Implement your humidity sensor reading code here
  return 60.0; // Example value
}

float readMoisture() {
  // Implement your soil moisture sensor reading code here
  int rawValue = analogRead(MOISTURE_PIN);
  return map(rawValue, 0, 4095, 0, 100);
}

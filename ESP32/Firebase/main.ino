#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <DHT.h>
#include <time.h>

// #include <Arduino.h>
// #if defined(ESP32)
//   #include <WiFi.h>
// #elif defined(ESP8266)
//   #include <ESP8266WiFi.h>
// #endif
// #include <Firebase_ESP_Client.h>
// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// //#include <Adafruit_BME280.h>

// // Provide the token generation process info.
// #include "addons/TokenHelper.h"
// // Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Firebase credentials
#define API_KEY "AIzaSyBdQDh-VkRQGcT_7T5ac9XvFVoeT0KH3UY"
#define DATABASE_URL "https://riego-iotech-default-rtdb.firebaseio.com"

// Objetos necesarios para Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Pines y sensores
#define WATER_PUMP_PIN 13    // Pin para la bomba de agua
#define DHT_PIN 4            // Pin para el sensor DHT
#define MOISTURE_PIN 36      // Pin para el sensor de humedad del suelo
#define DHTTYPE DHT22        // Tipo de sensor DHT

// Umbrales de humedad del suelo
#define SOIL_DRY 30          // Porcentaje considerado seco
#define SOIL_WET 70          // Porcentaje considerado húmedo

// Intervalos de tiempo
#define READING_INTERVAL 2000     // Intervalo entre lecturas (2 segundos)
#define HISTORY_INTERVAL 300000   // Intervalo para datos históricos (5 minutos)

// Inicialización del sensor DHT
DHT dht(DHT_PIN, DHTTYPE);

// Variable para rastrear el último momento en que se actualizó el historial
unsigned long lastHistoryUpdate = 0;

// Pin definitions
#define WATER_PUMP_PIN 13  // Pin donde se conecta la bomba de agua (GPIO 13)
#define DHT_PIN 4         //  Pin para conectar el sensor de temperatura y humedad (GPIO 4).
#define MOISTURE_PIN 36   // Pin para conectar el sensor de humedad del suelo (GPIO 36).

void setup()
{
    Serial.begin(115200);

    // Configurar el sensor DHT
    dht.begin();

    // Configurar el pin de la bomba de agua
    pinMode(WATER_PUMP_PIN, OUTPUT);
    digitalWrite(WATER_PUMP_PIN, LOW); // Asegurar que la bomba esté apagada inicialmente

    // Conectar a la red WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Configurar Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop()
{
    if (Firebase.ready()) {
        unsigned long currentMillis = millis();

        // Leer datos de los sensores
        float temperature = readTemperature();    // Leer la temperatura
        float humidity = readHumidity();          // Leer la humedad relativa
        float soilMoisture = readSoilMoisture();  // Leer la humedad del suelo

        // Actualizar datos actuales en Firebase
        updateCurrentReadings(temperature, humidity, soilMoisture);

        // Guardar datos históricos cada HISTORY_INTERVAL (5 minutos)
        if (currentMillis - lastHistoryUpdate >= HISTORY_INTERVAL) {
            updateHistoricalData(temperature, humidity, soilMoisture);
            lastHistoryUpdate = currentMillis;
        }

        // Verificar si hay comandos para activar/desactivar la bomba desde Firebase
        checkWateringCommands();
    }

    delay(READING_INTERVAL); // Esperar antes de realizar la siguiente iteración
}

// Función para actualizar lecturas actuales en Firebase
void updateCurrentReadings(float temperature, float humidity, float soilMoisture)
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/current";

    FirebaseJson json;
    json.set("temperature", temperature);  // Registrar temperatura
    json.set("humidity", humidity);        // Registrar humedad relativa
    json.set("soilMoisture", soilMoisture); // Registrar humedad del suelo
    json.set("timestamp", String(millis())); // Timestamp de la lectura

    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Current data updated successfully");
    } else {
        Serial.println("Failed to update current data");
        Serial.println("Reason: " + fbdo.errorReason());
    }
}

// Función para guardar datos históricos en Firebase
void updateHistoricalData(float temperature, float humidity, float soilMoisture)
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/history/" + String(millis());

    FirebaseJson json;
    json.set("temperature", temperature);
    json.set("humidity", humidity);
    json.set("soilMoisture", soilMoisture);
    json.set("timestamp", String(millis()));

    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Historical data updated successfully");
    } else {
        Serial.println("Failed to update historical data");
        Serial.println("Reason: " + fbdo.errorReason());
    }
}

// Función para verificar comandos desde Firebase (START/STOP)
void checkWateringCommands()
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/commands/water/action";

    if (Firebase.RTDB.getString(&fbdo, path.c_str())) {
        String action = fbdo.stringData();
        if (action == "START") {
            startWatering(); // Activar la bomba
        } else if (action == "STOP") {
            stopWatering(); // Desactivar la bomba
        }
    }
}

// Leer temperatura usando el sensor DHT
float readTemperature() {
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("Failed to read temperature!");
        return 0; // Valor por defecto en caso de error
    }
    return t;
}

// Leer humedad relativa usando el sensor DHT
float readHumidity() {
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("Failed to read humidity!");
        return 0; // Valor por defecto en caso de error
    }
    return h;
}

// Leer humedad del suelo usando el sensor analógico
float readMoisture() {
    int rawValue = analogRead(MOISTURE_PIN); // Leer el valor crudo del sensor
    float moisture = map(rawValue, 4095, 0, 0, 100); // Convertir a porcentaje
    moisture = constrain(moisture, 0, 100); // Limitar a 0-100%
    return moisture;
}

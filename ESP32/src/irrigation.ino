#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <DHT.h>
#include <time.h>

// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// //#include <Adafruit_BME280.h>

// Configuración de las credenciales de WiFi
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Configuración de las credenciales de Firebase
#define API_KEY ""
#define DATABASE_URL ""

// Objetos necesarios para interactuar con Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Definición de los pines y los sensores
#define WATER_PUMP_PIN 13    // Pin para la bomba de agua
#define DHT_PIN 4            // Pin para el sensor DHT
#define MOISTURE_PIN 36      // Pin para el sensor de humedad del suelo
#define DHTTYPE DHT22        // Tipo de sensor DHT

// Umbrales para determinar la humedad del suelo (en porcentaje)
#define SOIL_DRY 30          // Porcentaje considerado seco
#define SOIL_WET 70          // Porcentaje considerado húmedo

// Intervalos de tiempo para las lecturas y almacenamiento de datos
#define READING_INTERVAL 2000     // Intervalo entre lecturas (2 segundos)
#define HISTORY_INTERVAL 300000   // Intervalo para datos históricos (5 minutos)

// Inicialización del sensor DHT
DHT dht(DHT_PIN, DHTTYPE);

// Variable para almacenar el tiempo del último almacenamiento de datos históricos
unsigned long lastHistoryUpdate = 0;

// Configuración de los pines de la bomba de agua
#define WATER_PUMP_PIN 13 // Pin para la bomba de agua
#define DHT_PIN 4         // Pin para el sensor de temperatura y humedad (DHT)
#define MOISTURE_PIN 36   // Pin para el sensor de humedad del suelo

void setup()
{
    Serial.begin(115200);

    // Inicializa el sensor DHT
    dht.begin();

    // Configura el pin de la bomba de agua como salida y lo apaga inicialmente
    pinMode(WATER_PUMP_PIN, OUTPUT);
    digitalWrite(WATER_PUMP_PIN, LOW);  // La bomba está apagada inicialmente

    // Conexión a la red WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Configuración de Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop()
{
    // Verifica si Firebase está listo para recibir datos
    if (Firebase.ready()) {
        unsigned long currentMillis = millis();

        // Lee los datos de los sensores
        float temperature = readTemperature();    // Leer la temperatura
        float humidity = readHumidity();          // Leer la humedad relativa
        float soilMoisture = readSoilMoisture();  // Leer la humedad del suelo

        // Actualiza los datos actuales en Firebase
        updateCurrentReadings(temperature, humidity, soilMoisture);

        // Guarda los datos históricos cada 5 minutos
        if (currentMillis - lastHistoryUpdate >= HISTORY_INTERVAL) {
            updateHistoricalData(temperature, humidity, soilMoisture);
            lastHistoryUpdate = currentMillis;
        }

        // Verifica si hay comandos para iniciar o detener el riego desde Firebase
        checkWateringCommands();
    }

    delay(READING_INTERVAL); // Espera 2 segundos antes de realizar otra iteración
}

// Función para actualizar los datos actuales en Firebase
void updateCurrentReadings(float temperature, float humidity, float soilMoisture)
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/current";

    FirebaseJson json;
    json.set("temperature", temperature);  // Registrar temperatura
    json.set("humidity", humidity);        // Registrar humedad relativa
    json.set("soilMoisture", soilMoisture); // Registrar humedad del suelo
    json.set("timestamp", String(millis())); // Timestamp de la lectura

    // Actualiza los datos actuales en Firebase en la ruta especificada
    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Current data updated successfully");
    } else {
        Serial.println("Failed to update current data");
        Serial.println("Reason: " + fbdo.errorReason());
    }
}

// Función para almacenar datos históricos en Firebase
void updateHistoricalData(float temperature, float humidity, float soilMoisture)
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/history/" + String(millis());

    FirebaseJson json;
    json.set("temperature", temperature);
    json.set("humidity", humidity);
    json.set("soilMoisture", soilMoisture);
    json.set("timestamp", String(millis()));

    // Guarda los datos históricos en Firebase en la ruta especificada
    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Historical data updated successfully");
    } else {
        Serial.println("Failed to update historical data");
        Serial.println("Reason: " + fbdo.errorReason());
    }
}

// Función para verificar los comandos de riego desde Firebase (START/STOP)
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

// Función para leer la temperatura usando el sensor DHT
float readTemperature() {
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("Failed to read temperature!");
        return 0; // Valor por defecto en caso de error
    }
    return t;
}

// Función para leer la humedad relativa usando el sensor DHT
float readHumidity() {
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("Failed to read humidity!");
        return 0; // Valor por defecto en caso de error
    }
    return h;
}

// Función para leer la humedad del suelo usando el sensor de humedad
float readMoisture() {
    int rawValue = analogRead(MOISTURE_PIN); // Leer el valor crudo del sensor
    float moisture = map(rawValue, 4095, 0, 0, 100); // Convertir a porcentaje
    moisture = constrain(moisture, 0, 100); // Limitar a 0-100%
    return moisture;
}

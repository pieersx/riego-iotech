#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

#include "wifi_config.h"
#include "firebase_config.h"
#include "firebase.h"
#include "sensors.h"
#include "actuators.h"

// Configuración de intervalos
#define READING_INTERVAL 2000
#define HISTORY_INTERVAL 300000

// variables globales
unsigned long lastHistoryUpdate = 0;

void setup()
{
    Serial.begin(115200);

    // Inicializar sensores y actuadores
    setupSensors();
    setupActuators();

    // Conexión WiFi
    connectWifi();

    // Configuración de la base de datos Firebase
    setupFirebase();
}

void loop()
{
    if (Firebase.ready()) {
        unsigned long currentMillis = millis();

        // Leer datos de los sensores
        float temperature = readTemperature();
        float humidity = readHumidity();
        float soilMoisture = readSoilMoisture();

        // Actualizar lecturas actuales en Firebase
        updateCurrentReadings(temperature, humidity, soilMoisture);

        // Guardar datos históricos cada HISTORY_INTERVAL (5 minutos)
        if (currentMillis - lastHistoryUpdate >= HISTORY_INTERVAL) {
            updateHistoricalData(temperature, humidity, soilMoisture);
            lastHistoryUpdate = currentMillis;
        }

        // Verificar comandos desde Firebase
        checkWateringCommands();
    }

    delay(READING_INTERVAL); // Esperar antes de realizar la siguiente iteración
}

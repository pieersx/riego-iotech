#include "sensors.h"

DHT dht(DHT_PIN, DHTTYPE);

void setupSensors() {
    dht.begin();
}

float readTemperature()
{
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("¡Error al leer la temperatura!");
        return 0;
    }
    return t;
}

float readHumidity()
{
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("¡Error al leer la humedad!");
        return 0;
    }
    return h;
}

float readSoilMoisture()
{
    int rawValue = analogRead(MOISTURE_PIN);
    float moisture = map(rawValue, 4095, 0, 0, 100);
    return constrain(moisture, 0, 100);
}

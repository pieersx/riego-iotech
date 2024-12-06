#ifndef SENSORS
#define SENSORS

#include <DTH.h>

#define DHT_PIN 4
#define DHTTYPE DHT22
#define MOISTURE_PIN 36

void setupSensors();
float readTemperature();
float readHumidity();
float readSoilMoisture();

#endif // SENSORS
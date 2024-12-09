#ifndef FIREBASE
#define FIREBASE

#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>

void updateCurrentReadings(float, float, float);
void updateHistoricalData(float, float, float);
void checkWateringCommands();

#endif // FIREBASE

#include "firebase_config.h"
#include "firebase.h"
#include "actuators.h"

void updateCurrentReadings(float temperature, float humidity, float soilMoisture)
{
    FirebaseJson json;
    json.set("temperature", temperature);
    json.set("humidity", humidity);
    json.set("soilMoisture", soilMoisture);
    json.set("timestamp", String(millis()));

    if (!Firebase.RTDB.setJson(&fbdo, "/current", &json)) {
        Serial.println("Error al actualizar los datos actuales: " + fbdo.errorReason());
    }
}

void updateHistoricalData(float temperature, float humidity, float soilMoisture)
{
    FirebaseJson json;
    json.set("temperature", temperature);
    json.set("humidity", humidity);
    json.set("soilMoisture", soilMoisture);
    json.set("timestamp", String(millis()));

    if (!Firebase.RTDB.pushJSON(&fbdo, "/history", &json)) {
        Serial.println("Error al actualizar los datos históricos: " + fbdo.errorReason());
    }
}

void checkWateringCommands()
{
    if (Firebase.RTDB.getString(&fbdo, "/commands/water/action")) {
        String action = fbdo.stringData();
        if (action == "START") startWatering();
        if (action == "STOP") stopWatering();
    }
}

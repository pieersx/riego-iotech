#ifndef FIREBASE_CONFIG
#define FIREBASE_CONFIG

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define API_KEY ""
#define DATABASE_URL ""

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setupFirebase()
{
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    if (Firebase.ready()) {
        Serial.println("Firebase listo");
    } else {
        Serial.println("Error de conexión con Firebase");
    }
}

#endif // FIREBASE_CONFIG_H

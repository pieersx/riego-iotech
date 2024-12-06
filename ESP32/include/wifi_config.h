#ifndef WIFI_CONFIG
#define WIFI_CONFIG

#include <WiFi.h>

const char *ssid = "WIFI_SSID";
const char *password = "WIFI_PASSWORD";

void connectWifi() {
    WiFi.begin(ssid, password);
    while (WiFi.status()!= WL_CONNECTED) {
        delay(1000);
        Serial.print("Connecting to WiFi...");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

#endif // WIFI_CONFIG

#ifndef WIFI_CONFIG
#define WIFI_CONFIG

#include <WiFi.h>

const char *ssid = "WIFI_SSID";
const char *password = "WIFI_PASSWORD";

void connectWifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status()!= WL_CONNECTED) {
        delay(1000);
        Serial.print("Connectando a WiFi...");
    }
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

#endif // WIFI_CONFIG_H

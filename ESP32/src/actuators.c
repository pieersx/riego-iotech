#include <Arduino.h>

#include "actuators.h"

void setupActuators()
{
    pinMode(WATER_PUMP_PIN, OUTPUT);
    digitalWrite(WATER_PUMP_PIN, LOW); // Asegurar que la bomba esté apagada inicialmente
}

void startWatering()
{
    digitalWrite(WATER_PUMP_PIN, HIGH);
    Serial.println("Comienzo del riego");
}

void stopWatering()
{
    digitalWrite(WATER_PUMP_PIN, LOW);
    Serial.println("El riego se detuvo");
}

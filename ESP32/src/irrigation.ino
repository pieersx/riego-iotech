#include <WiFi.h>

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#include <ArduinoJson.h>
#include <DHT.h>

// Configuración de las credenciales de WiFi
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// Configuración de las credenciales de Firebase
#define API_KEY ""
#define DATABASE_URL ""

// Objetos necesarios para interactuar con Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Definición de los pines y los sensores
#define PIN_BOMBA_AGUA 13       // Pin para la bomba de agua
#define PIN_HUMEDAD    36       // Pin para el sensor de humedad del suelo
#define PIN_DHT         4       // Pin para el sensor DHT
#define TIPO_DHT        DHT22   // Tipo de sensor DHT

// Umbrales para determinar la humedad del suelo (en porcentaje)
#define HUMEDAD_SECA   30          // Porcentaje considerado seco
#define HUMEDAD_HUMEDA 70          // Porcentaje considerado húmedo

// Intervalos de tiempo para las lecturas y almacenamiento de datos
#define INTERVALO_LECTURA   2000     // Intervalo entre lecturas (2 segundos)
#define INTERVALO_HISTORIAL 300000   // Intervalo para datos históricos (5 minutos)

// Inicialización del sensor DHT
DHT dht(PIN_DHT, TIPO_DHT);

// Variable para controlar el tiempo del último registro histórico
unsigned long ultimaActualizacionHistorial = 0;

void setup()
{
    Serial.begin(115200);

    // Inicializa el sensor DHT
    dht.begin();

    // Configura el pin de la bomba de agua como salida y lo apaga inicialmente
    pinMode(PIN_BOMBA_AGUA, OUTPUT);
    digitalWrite(PIN_BOMBA_AGUA, LOW);  // La bomba está apagada inicialmente

    // Conexión a la red WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("WiFi conectado");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configuración de Firebase
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

void loop()
{
    // Verifica si Firebase está listo para recibir datos
    if (Firebase.ready()) {
        unsigned long tiempoActual = millis();

        // Lee los datos de los sensores
        float temperatura = leerTemperatura();    // Leer la temperatura
        float humedad = leerHumedad();          // Leer la humedad relativa
        float humedadSuelo = leerHumedadSuelo();  // Leer la humedad del suelo

        // Verificar si se necesita riego automático
        // verificarRiegoAutomatico(humedadSuelo);

        // Actualiza los datos actuales en Firebase
        actualizarLecturasActuales(temperatura, humedad, humedadSuelo);

        // Guarda los datos históricos cada 5 minutos
        if (tiempoActual - ultimaActualizacionHistorial >= INTERVALO_HISTORIAL) {
            actualizarDatosHistoricos(temperatura, humedad, humedadSuelo);
            ultimaActualizacionHistorial = tiempoActual;
        }

        // Verifica si hay comandos para iniciar o detener el riego desde Firebase
        verificarComandosRiego();
    }

    delay(INTERVALO_LECTURA); // Espera 2 segundos antes de realizar otra iteración
}

// Función para actualizar los datos actuales en Firebase
void actualizarLecturasActuales(float temperatura, float humedad, float humedadSuelo)
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/current";

    FirebaseJson json;
    json.set("temperature", temperatura);  // Registrar temperatura
    json.set("humidity", humedad);        // Registrar humedad relativa
    json.set("soilMoisture", humedadSuelo); // Registrar humedad del suelo
    json.set("timestamp", String(millis())); // Timestamp de la lectura

    // Actualiza los datos actuales en Firebase en la ruta especificada
    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Datos actuales actualizados correctamente");
    } else {
        Serial.println("Error al actualizar los datos actuales");
        Serial.println("Error: " + fbdo.errorReason());
    }
}

// Función para almacenar datos históricos en Firebase
void actualizarDatosHistoricos(float temperatura, float humedad, float humedadSuelo)
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/history/" + String(millis());

    FirebaseJson json;
    json.set("temperature", temperatura);
    json.set("humidity", humedad);
    json.set("soilMoisture", humedadSuelo);
    json.set("timestamp", String(millis()));

    // Guarda los datos históricos en Firebase en la ruta especificada
    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("Datos históricos actualizados correctamente");
    } else {
        Serial.println("Error al actualizar los datos históricos");
        Serial.println("Error: " + fbdo.errorReason());
    }
}

// Función para verificar los comandos de riego desde Firebase (START/STOP)
void verificarComandosRiego()
{
    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/commands/water/action";

    if (Firebase.RTDB.getString(&fbdo, path.c_str())) {
        String action = fbdo.stringData();
        if (action == "START") iniciarRiego();      // Activar la bomba
        else if (action == "STOP") detenerRiego();  // Desactivar la bomba
    }
}

// Función para leer la temperatura usando el sensor DHT
float leerTemperatura()
{
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("Error al leer la temperatura");
        return 0; // Valor por defecto en caso de error
    }
    return t;
}

// Función para leer la humedad relativa usando el sensor DHT
float leerHumedad()
{
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("Error al leer la humedad");
        return 0; // Valor por defecto en caso de error
    }
    return h;
}

// Función para leer la humedad del suelo usando el sensor de humedad
float leerHumedadSuelo()
{
    int valorBruto = analogRead(PIN_HUMEDAD);              // Leer el valor crudo del sensor
    float humedadSuelo = map(valorBruto, 4095, 0, 0, 100); // Convertir a porcentaje
    humedadSuelo = constrain(humedadSuelo, 0, 100);        // Limitar a 0-100%
    return humedadSuelo;
}

// Función para iniciar el riego
void iniciarRiego()
{
    digitalWrite(PIN_BOMBA_AGUA, HIGH); // Encender bomba de agua
    Serial.println("Riego iniciado");

    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/watering";
    FirebaseJson json;
    json.set("status", "active");
    json.set("startTime", String(millis()));
    Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json);
}

// Función para detener el riego
void detenerRiego()
{
    digitalWrite(PIN_BOMBA_AGUA, LOW);  // Apagar bomba de agua
    Serial.println("Riego detenido");

    String path = "/UsersData/" + String(auth.token.uid.c_str()) + "/watering";
    FirebaseJson json;
    json.set("status", "inactive");
    json.set("stopTime", String(millis()));
    Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json);
}

// Nueva función para control automático de riego
void verificarRiegoAutomatico(float humedadSuelo) {
    String autoPath = "/UsersData/" + String(auth.token.uid.c_str()) + "/settings/autoWatering";

    // Verificar si el riego automático está activado
    if (Firebase.RTDB.getBool(&fbdo, autoPath.c_str()) && fbdo.boolData()) {
        if (humedadSuelo <= HUMEDAD_SECA) {
            // Si el suelo está muy seco, iniciar riego
            iniciarRiego();

            // Programar detención después de 10 segundos
            delay(10000);
            detenerRiego();

            // Esperar 1 hora antes de verificar nuevamente
            delay(3600000);
        } else if (humedadSuelo >= HUMEDAD_HUMEDA) {
            // Si el suelo está suficientemente húmedo, asegurar que la bomba esté apagada
            detenerRiego();
        }
    }
}

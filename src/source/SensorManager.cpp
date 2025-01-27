#include "../header/SensorManager.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_SGP30.h>
#include <HTTPClient.h>
#include "../header/TimestampManager.h"

// Définir les pins et le type de capteur
#define DHTPIN 12
#define DHTTYPE DHT22
String salle = "Secrétariat";
String nomsa = "ESP-026";

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_SGP30 sgp;

float currentTemperature = NAN;
float currentHumidity = NAN;
uint16_t currentCO2 = 0;

const char* serverName = "https://sae34.k8s.iut-larochelle.fr/api/captures";

/**
* @brief Initialise les capteurs DHT et SGP30.
*
* Cette fonction initialise les capteurs connectés au système.
* - Elle appelle `dht.begin()` pour démarrer le capteur DHT.
* - Elle vérifie si le capteur SGP30 est disponible avec `sgp.begin()`.
*
* @return void
*/
void setupSensors() {
    dht.begin();
    if (!sgp.begin()) {
        Serial.println("Erreur : SGP30 non trouvé.");
        while (1);
    }
}
     
void readSensorCO2Data(){
    if (sgp.IAQmeasure()) {
        currentCO2 = sgp.eCO2;
    }
}

/**
 * @brief Lit les données de température et d'humidité depuis un 
 * capteur DHT et les stocke dans les variables `currentTemperature` et `currentHumidity`.
 *
 * Cette fonction interroge le capteur DHT pour récupérer les événements de température et 
 * d'humidité, puis vérifie si les valeurs sont valides. 
 * Si la température et l'humidité sont valides (non "NaN"), elles sont assignées 
 * respectivement aux variables globales `currentTemperature` et `currentHumidity`.
 * 
 * @return void
 */
void readSensorTandHData() {
    sensors_event_t humidityEvent, temperatureEvent;

    // Obtient les événements de température et d'humidité du capteur DHT
    dht.temperature().getEvent(&temperatureEvent);
    dht.humidity().getEvent(&humidityEvent);

    // Si la température est valide, la stocke dans la variable globale
    if (!isnan(temperatureEvent.temperature)) {
        currentTemperature = temperatureEvent.temperature;
    }
    // Si l'humidité est valide, la stocke dans la variable globale
    if (!isnan(humidityEvent.relative_humidity)) {
        currentHumidity = humidityEvent.relative_humidity;
    }
}



float getCurrentTemperature() {
    return currentTemperature;
}

/**
 * @brief Récupère l'humidité actuelle 
 * stockée dans la variable globale `currentHumidity`.
 *
 * Cette fonction retourne la valeur de l'humidité actuelle, 
 * qui est stockée dans la variable globale `currentHumidity`. 
 * Si l'humidité a été mise à jour par la fonction `readSensorTandHData()`, 
 * cette valeur sera celle la plus récente.
 * 
 * @return L'humidité actuelle en pourcentage (float).
 */
float getCurrentHumidity() {
    return currentHumidity;
}

uint16_t getCurrentCO2() {
    return currentCO2;
}

void sendAPICO2()
{
    HTTPClient http;

    http.begin(serverName);

    http.addHeader("accept", "application/ld+json");
    http.addHeader("dbname", "sae34bdl2eq1");
    http.addHeader("username", "l2eq1");
    http.addHeader("userpass", "Bemvat-taxxoj-1jyrbi");
    http.addHeader("Content-Type", "application/ld+json");

    int httpResponseCode =http.POST("{\"nom\":\"co2\",\"valeur\":\"" + String(currentCO2) + "\",\"dateCapture\":\"" + String(getFormattedTimestamp()) + "\", \"localisation\":\""+ salle + "\", \"nomsa\":\""+ nomsa + "\", \"description\":\"Test envoi des données capteurs CO2\" }");

    Serial.println("{\"nom\":\"co2\",\"valeur\":\"" + String(currentCO2) + "\",\"dateCapture\":" + String(getFormattedTimestamp()) + "\", \"localisation\":" + salle + "\", \"nomsa\":" + nomsa + "\", \"description\":\"Envoi des données capteurs CO2\" }");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
}

void sendAPITemp()
{
    HTTPClient http;

    http.begin(serverName);

    http.addHeader("accept", "application/ld+json");
    http.addHeader("dbname", "sae34bdl2eq1");
    http.addHeader("username", "l2eq1");
    http.addHeader("userpass", "Bemvat-taxxoj-1jyrbi");
    http.addHeader("Content-Type", "application/ld+json");

    int httpResponseCode =http.POST("{\"nom\":\"temp\",\"valeur\":\"" + String(currentTemperature) + "\",\"dateCapture\":\"" + String(getFormattedTimestamp()) + "\", \"localisation\":\""+ salle + "\", \"nomsa\":\""+ nomsa + "\", \"description\":\"Test envoi des données capteurs Temp\" }");

    Serial.println("{\"nom\":\"temp\",\"valeur\":\"" + String(currentTemperature) + "\",\"dateCapture\":" + String(getFormattedTimestamp()) + "\", \"localisation\":" + salle + "\", \"nomsa\":" + nomsa + "\", \"description\":\"Envoi des données capteurs Temp\" }");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
}

void sendAPIHum()
{
    HTTPClient http;

    http.begin(serverName);

    http.addHeader("accept", "application/ld+json");
    http.addHeader("dbname", "sae34bdl2eq1");
    http.addHeader("username", "l2eq1");
    http.addHeader("userpass", "Bemvat-taxxoj-1jyrbi");
    http.addHeader("Content-Type", "application/ld+json");

    int httpResponseCode =http.POST("{\"nom\":\"hum\",\"valeur\":\"" + String(currentHumidity) + "\",\"dateCapture\":\"" + String(getFormattedTimestamp()) + "\", \"localisation\":\""+ salle + "\", \"nomsa\":\""+ nomsa + "\", \"description\":\"Envoi des données capteurs Hum\" }");

    Serial.println("{\"nom\":\"hum\",\"valeur\":\"" + String(currentHumidity) + "\",\"dateCapture\":" + String(getFormattedTimestamp()) + "\", \"localisation\":" + salle + "\", \"nomsa\":" + nomsa + "\", \"description\":\"Envoi des données capteurs hum\" }");
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Free resources
    http.end();
}

void SensorCO2Task(void* parameter) {
    for (;;) {
        readSensorCO2Data();
        sendAPICO2(); 
        vTaskDelay(60000*10 / portTICK_PERIOD_MS); // Lecture toutes les 5 secondes
    }
}

void SensorTandHTask(void* parameter) {
    for (;;) {
        readSensorTandHData();
        sendAPITemp();
        sendAPIHum();
        vTaskDelay(60000*10 / portTICK_PERIOD_MS); // Lecture toutes les 5 secondes
    }
}


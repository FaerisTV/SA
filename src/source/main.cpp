#include <Arduino.h>
#include "../header/WifiManager.h"
#include "../header/SensorManager.h"
#include "../header/DisplayManager.h"
#include "../header/TimestampManager.h"
#include <Adafruit_SSD1306.h>

// Variables globales
unsigned long lastDataReadTime = 0; 
const unsigned long dataReadInterval = 5000;

unsigned long lastWiFiRetryTime = 0; // Dernière tentative de reconnexion
const unsigned long wifiRetryInterval = 2 * 60 * 1000; // Intervalle de 2 minutes en millisecondes

TaskHandle_t xWiFiTask;

void setup() {
    Serial.begin(9600);
    Serial.println(F("Démarrage"));

    // Déconnexion initiale du Wi-Fi
    disconnectWifi();

    // Crée la tâche Wi-Fi et attend sa fin avant de lancer les autres tâches
    xTaskCreate(WiFiTask, "WiFi Task", 2000, NULL, 1, &xWiFiTask);

    // Initialisation des composants
    setupSensors();     // Initialisation des capteurs (DHT, SGP30, etc.)
    setupDisplay();     // Initialisation de l'écran OLED
    // setupMotion();      // Initialisation du capteur de mouvement PIR
    setupTimestamp();   // Initialisation de la gestion de l'horodatage (RTC)

    // Création des tâches FreeRTOS
    xTaskCreate(SensorCO2Task, "Sensor CO2 Task", 4000, NULL, 1, NULL);
    xTaskCreate(SensorTandHTask, "Sensor Humidity Task", 4000, NULL, 1, NULL);
    xTaskCreate(TimestampTask, "Timestamp Task", 2000, NULL, 1, NULL);
    xTaskCreate(DisplayTask, "Display Task", 10000, NULL, 1, NULL);

    Serial.println("Toutes les tâches sont maintenant en cours d'exécution.");
}

void loop() {
    // Gestion du mouvement (allumage/éteignage de l'écran)
    // handleMotion();

    // Lecture périodique des capteurs
    if (millis() - lastDataReadTime >= dataReadInterval) {
        lastDataReadTime = millis();

        // Lecture et mise à jour des données des capteurs
        readSensorCO2Data();  // Lecture des données de CO2
        readSensorTandHData(); // Lecture des données de température et d'humidité

        // Mise à jour de l'affichage (via DisplayManager)
        updateDisplay();
    }

    // Reconnexion Wi-Fi si nécessaire (via WifiManager)
    handleWiFiReconnect();
}

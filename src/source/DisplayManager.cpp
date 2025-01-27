#include "../header/DisplayManager.h"
#include "../header/WifiManager.h"
#include <WiFi.h> // Wifi library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "../header/SensorManager.h"
#include "../header/TimestampManager.h"

// Définir les pins et le type de capteur
#define PIR_SENSOR_PIN 18   // GPIO18 pour le capteur PIR

// Initialisation de l'écran OLED SSD1306 (128x64 pixels)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C  // Vérifiez si c'est bien l'adresse de votre écran

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Définir l'écran OLED

// Variables pour contrôler l'état de l'écran
bool screenOn = false;
unsigned long screenOnTime = 0;
unsigned long lastMotionTime = 0;   // Dernière détection de mouvement
unsigned long motionCooldown = 20000; // 20 secondes de cooldown avant d'éteindre l'écran

// Variables pour gérer l'affichage de l'heure
unsigned long lastClockUpdateTime = 0; // Dernier temps de mise à jour de l'heure
const unsigned long clockUpdateInterval = 1000; // Intervalle de 1 seconde

DateTime now;

void setupDisplay() {
    Serial.println("Initialisation de l'écran OLED...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("Erreur : écran OLED non trouvé. Vérifiez les connexions.");
        while (1);  // Bloque le programme si l'écran ne peut pas être trouvé
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Demarrage..."));
    display.display();
    delay(2000);  // Petit délai pour afficher le message d'initialisation

    display.ssd1306_command(0xAE); // Commande pour éteindre l'écran OLED
}

void updateDisplay() {
    display.clearDisplay();
    
    unsigned long currentMillis = millis();
   
    // Gérer le capteur PIR
    bool motionDetected = digitalRead(PIR_SENSOR_PIN) == HIGH;
    if (motionDetected) {
        lastMotionTime = millis();  // Met à jour le temps du dernier mouvement
        if (!screenOn) {
            screenOn = true;
            display.ssd1306_command(0xAF); // Allume l'écran si mouvement détecté
            screenOnTime = millis();
        }
    }

    // Si l'écran est allumé et qu'il n'y a pas eu de mouvement pendant 20 secondes, éteindre l'écran
    if (screenOn && (millis() - lastMotionTime >= motionCooldown) && (millis() - screenOnTime >= 20000)) {
        screenOn = false;
        display.ssd1306_command(0xAE); // Éteint l'écran après 20 secondes sans mouvement
    }

    // Affichage du statut de la connexion Wi-Fi
    display.setTextSize(1);
    display.setCursor(0, 0);

    // Récupération de l'heure actuelle
    now = getCurrentTimestamp();

    // Affichage de la date
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Date: "));
    if (now.day() < 10)
    {
        display.print(F("0"));
    } 
    display.print(now.day());
    display.print(F("/"));
    if (now.month() < 10)
    {
        display.print(F("0"));  
    } 
    display.print(now.month());
    display.print(F("/"));
    display.print(now.year());

    // Affichage de l'heure
    display.setCursor(0, 8);
    display.print(F("Heure: "));
    if (now.hour() < 10)
    {
        display.print(F("0"));
    } 
    display.print(now.hour());
    display.print(F(":"));
    if (now.minute() < 10)
    {
        display.print(F("0"));
    } 
    display.print(now.minute());
    display.print(F(":"));
    if (now.second() < 10)
    {
        display.print(F("0"));
    } 
    display.print(now.second());

    // Affichage des capteurs
    display.setCursor(0, 22);
    display.print(F("Temp: "));
    display.print(getCurrentTemperature());
    display.print(F(" C"));

    display.setCursor(0, 34);
    display.print(F("Hum: "));
    display.print(getCurrentHumidity());
    display.print(F(" %"));

    display.setCursor(0, 46);
    display.print(F("CO2: "));
    display.print(getCurrentCO2());
    display.print(F(" ppm"));

    Serial.print("Temperature: ");
    Serial.print(getCurrentTemperature());
    Serial.print(" C, Humidite: ");
    Serial.print(getCurrentHumidity());
    Serial.print(" %, CO2: ");
    Serial.print(getCurrentCO2());
    Serial.println(" ppm");

    display.setCursor(0, 55);
    if (isWiFiConnected()) {
        display.print(F("Wi-Fi: Oui"));
    } 
    else if (isWiFiConnecting()){
        display.print(F("Wi-Fi: Connexion..."));
    }
    else {
        display.print(F("Wi-Fi: Non"));
    }
    display.display();
}

void DisplayTask(void* parameter) {
    for (;;) {
        updateDisplay();
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Mise à jour toutes les secondes
    }
}

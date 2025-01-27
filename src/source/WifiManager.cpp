#include "../header/WifiManager.h"
#include <WiFi.h>
#include "esp_wpa2.h"
#include <Adafruit_SSD1306.h>

// Définir vos informations Eduroam
#define EAP_IDENTITY "username"
#define EAP_USERNAME "username"
#define EAP_PASSWORD "mot_de_passe"

const char* ssid = "eduroam";

bool wifiConnected = false;
bool wifiConnecting = false;
bool wifiInitialization = false;
bool wifiScanComplete = false;

unsigned long lastWifiReconnectTime = 0; // Dernier temps de tentative de reconnexion
const unsigned long reconnectInterval = 60000; // Intervalle de reconnexion en millisecondes (1 minutes)

void WiFiTask(void* pvParameters)
{
    // Initialize WiFi here
    setupWiFi();

    Serial.println("WiFi Task complete.");

    if(WiFi.isConnected())
    {
        vTaskDelete(NULL);
    }
}

/**
 * @brief Configure et établit une connexion Wi-Fi au réseau spécifié.
 *
 * Cette fonction tente de se connecter au réseau Wi-Fi défini par les variables 
 * globales `ssid`, `WPA2_AUTH_PEAP`, `EAP_IDENTITY`, 
 * `EAP_USERNAME` et `EAP_PASSWORD`. Elle configure l'ESP8266/ESP32 pour 
 * fonctionner en mode station (client), puis tente de se 
 * connecter à un réseau Eduroam ou tout autre réseau compatible. 
 * Si la connexion échoue, la fonction passe en mode hors ligne après 
 * un délai de 1 minute.
 * 
 * Elle surveille également le statut de la connexion Wi-Fi et affiche des 
 * informations sur la progression et l'état actuel dans 
 * le moniteur série.
 * 
 * @return void
 */
void setupWiFi() 
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    
    Serial.print("Connexion au réseau: ");
    Serial.println(ssid);
    WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD); // Connexion à Eduroam
    unsigned long startMillis = millis();
    wifiConnecting = true;  // Connexion en cours

    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startMillis >= 15000) { // Timeout de 15 secondes
            Serial.println("Connexion échouée. Passer en mode hors ligne.");
            wifiConnected = false;
            wifiConnecting = false;
            return;
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);

        // Mettre à jour l'état du scan Wi-Fi
        switch (WiFi.status()) {
            case WL_IDLE_STATUS:
                wifiScanComplete = false;
                Serial.println("Statut WiFi : Inactif, en attente d'action.");
                break;
            case WL_SCAN_COMPLETED:
                wifiScanComplete = true;
                Serial.println("Statut WiFi : Scan terminé, réseaux disponibles trouvés.");
                break;
            case WL_CONNECTED:
                wifiConnected = true;
                Serial.println("Statut WiFi : Connecté avec succès au réseau.");
                break;
            case WL_NO_SSID_AVAIL:
                wifiConnected = false;
                Serial.println("Statut WiFi : Aucun réseau SSID disponible.");
                break;
            case WL_CONNECT_FAILED:
                wifiConnected = false;
                Serial.println("Statut WiFi : Échec de connexion au réseau.");
                break;
            case WL_CONNECTION_LOST:
                wifiConnected = false;
                Serial.println("Statut WiFi : Connexion perdue avec le réseau.");
                break;
            case WL_DISCONNECTED:
                wifiConnected = false;
                Serial.println("Statut WiFi : Déconnecté du réseau.");
                break;
            default:
                Serial.println("Statut WiFi : Inconnu.");
                break;
        }
    }

    wifiConnecting = false;  // Connexion terminée
    Serial.println();
    Serial.print("Connecté à : ");
    Serial.println(ssid);
}

void disconnectWifi()
{
    WiFi.disconnect(true);
}

bool isConnected()
{
    return WiFi.isConnected();
}

/**
 * @brief Gère la reconnexion Wi-Fi automatique lorsque la connexion échoue.
 *
 * Cette fonction vérifie si la connexion Wi-Fi est perdue et si le temps écoulé 
 * depuis la dernière tentative de reconnexion 
 * est supérieur à l'intervalle spécifié. Si ces conditions sont remplies, 
 * elle tente de reconnecter le dispositif en appelant 
 * la fonction `setupWiFi()`. Un indicateur est utilisé pour éviter les 
 * tentatives de reconnexion en boucle.
 * 
 * @return void
 */
void handleWiFiReconnect() {
    if (!wifiConnected && !wifiConnecting && (millis() - lastWifiReconnectTime >= reconnectInterval))
    {
        wifiConnecting = true; // Indicateur pour éviter la reconnexion en boucle
        setupWiFi(); // Tente de se reconnecter
        wifiConnecting = false; // Une fois la tentative terminée, on réinitialise l'indicateur
    }
    vTaskDelete(NULL);
}


bool isWiFiConnected() {
    return wifiConnected;
}

bool isWiFiConnecting() {
    return wifiConnecting;
}

bool isWiFiInitialization() {
    return wifiInitialization;
}

bool isWiFiScanComplete() {
    return wifiScanComplete;
}

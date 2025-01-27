#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

extern bool wifiConnected;
extern bool wifiConnecting;
extern bool wifiInitialization;
extern bool wifiScanComplete;
extern unsigned long lastWifiReconnectTime;
extern const unsigned long reconnectInterval;

void setupWiFi();
void handleWiFiReconnect();
void WiFiTask(void* parameter);
void disconnectWifi();
bool isConnected();

bool isWiFiConnected();  // Récupère l'état de la connexion Wi-Fi
bool isWiFiConnecting();  // Récupère l'état de la connexion en cours
bool isWiFiInitialization();  // Récupère l'état d'initialisation
bool isWiFiScanComplete();  // Récupère l'état du scan des réseaux Wi-Fi
#endif

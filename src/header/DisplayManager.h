#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>

// Déclaration des variables externes
extern bool screenOn;              // Déclarer comme externe
extern unsigned long screenOnTime; // Déclarer comme externe


void setupDisplay();
void updateDisplay();
void DisplayTask(void* parameter);

#endif // DISPLAY_MANAGER_H

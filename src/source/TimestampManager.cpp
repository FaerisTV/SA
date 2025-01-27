#include "../header/TimestampManager.h"

// Instance globale pour le module RTC
RTC_DS3231 rtc;

// Initialisation du module d'horodatage
void setupTimestamp() {
    if (!rtc.begin()) {
        Serial.println(F("Erreur : RTC non trouvé !"));
        while (1); // Bloquer si la RTC n'est pas détectée
    }

    if (rtc.lostPower()) {
        Serial.println(F("RTC a perdu l'alimentation, réglage de l'heure avec l'heure de compilation."));
        // Ajuster l'heure UNIQUEMENT si la RTC a perdu l'alimentation
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
        Serial.println(F("RTC initialisée et déjà configurée."));
    }

    Serial.println(F("RTC initialisée avec succès."));
}

// Tâche pour gérer l'horodatage
void TimestampTask(void* parameter) {
    for (;;) {
        DateTime now = rtc.now();

        String formattedTimestamp = getFormattedTimestamp();

        Serial.println(formattedTimestamp);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Pause de 1 seconde
    }
}

String getFormattedTimestamp() {
    DateTime now = rtc.now();
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    return String(buffer);
}

// Récupérer l'heure actuelle
DateTime getCurrentTimestamp() {
    return rtc.now();
}

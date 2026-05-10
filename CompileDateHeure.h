// CompileDateHeure.h
// Version : 1.0.0
// Auteur  : FOURNIT Olivier
// Email   : olivier.fournet@free.fr
// GitHub  : https://github.com/Fo170/CompileDateHeure
// Date    : 2025
//
// Licence : GNU General Public License v3.0
//
// Librairie Arduino permettant d'afficher la date et l'heure de compilation.
// Compatible : ESP8266, ESP32, Arduino IDE, PlatformIO
//
// Utilisation :
//   #include <CompileDateHeure.h>
//   void setup() {
//       Serial.begin(9600);
//       printCompileDate();
//   }

#ifndef COMPILE_DATE_HEURE_H
#define COMPILE_DATE_HEURE_H

#include <Arduino.h>

// ----- Données de compilation -----
const char* COMPILE_DATE = __DATE__;  // Format : "Mmm dd yyyy" (ex: "May 10 2026")
const char* COMPILE_TIME = __TIME__;  // Format : "hh:mm:ss"  (ex: "14:30:00")

// ----- N° de version -----
#define COMPILE_DATE_HEURE_VERSION "1.0.0"

// ----- Constantes de format -----
#define FORMAT_DATE_NUMERIQUE     0  // "dd/mm/yyyy"
#define FORMAT_DATE_TEXTE_EN      1  // "dd/Mmm/yyyy" (anglais)
#define FORMAT_DATE_TEXTE_FR      2  // "dd/Mmm_fr/yyyy" (français)

// ----- Noms des mois (index 0 = Jan) -----
const char* MOIS_EN[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                            "Jul","Aug","Sep","Oct","Nov","Dec"};
const char* MOIS_FR[12] = {"Jan","Fév","Mar","Avr","Mai","Jui",
                            "Jui","Aoû","Sep","Oct","Nov","Déc"};

// ----- Chaîne de version globale -----
String s_versionCompilation;

// ----- Obtenir la date formatée -----
// format : FORMAT_DATE_NUMERIQUE (0), FORMAT_DATE_TEXTE_EN (1), FORMAT_DATE_TEXTE_FR (2)
inline String getBuildDate(int format = FORMAT_DATE_NUMERIQUE) {
    // Extraction du jour (position 4, 1 ou 2 chiffres)
    int day = (COMPILE_DATE[4] == ' ')
              ? (COMPILE_DATE[5] - '0')
              : ((COMPILE_DATE[4] - '0') * 10 + (COMPILE_DATE[5] - '0'));

    // Recherche du mois par les 3 premiers caractères
    int month = 0;
    int monthIdx = 0;
    for (int i = 0; i < 12; i++) {
        if (COMPILE_DATE[0] == MOIS_EN[i][0] &&
            COMPILE_DATE[1] == MOIS_EN[i][1] &&
            COMPILE_DATE[2] == MOIS_EN[i][2]) {
            month = i + 1;
            monthIdx = i;
            break;
        }
    }

    // Extraction de l'année (position 7-10)
    int year = (COMPILE_DATE[7] - '0') * 1000
             + (COMPILE_DATE[8] - '0') * 100
             + (COMPILE_DATE[9] - '0') * 10
             + (COMPILE_DATE[10] - '0');

    char buf[20];
    switch (format) {
        case FORMAT_DATE_TEXTE_EN:
            snprintf(buf, sizeof(buf), "%02d/%s/%04d", day, MOIS_EN[monthIdx], year);
            break;
        case FORMAT_DATE_TEXTE_FR:
            snprintf(buf, sizeof(buf), "%02d/%s/%04d", day, MOIS_FR[monthIdx], year);
            break;
        case FORMAT_DATE_NUMERIQUE:
        default:
            snprintf(buf, sizeof(buf), "%02d/%02d/%04d", day, month, year);
            break;
    }
    return String(buf);
}

// ----- Initialiser la chaîne de version : "dd/mm/yyyy-hh:mm:ss" -----
inline void initVersionCompilation(int format = FORMAT_DATE_NUMERIQUE) {
    s_versionCompilation = getBuildDate(format) + "-" + String(COMPILE_TIME);
}

// ----- Afficher la date et l'heure de compilation sur Serial -----
inline void printCompileDate() {
    Serial.println("========================================");
    Serial.println("   Compile Date & Time");
    Serial.println("========================================");
    Serial.print("  Date : ");
    Serial.println(getBuildDate(FORMAT_DATE_NUMERIQUE));
    Serial.print("  Time : ");
    Serial.println(COMPILE_TIME);
    Serial.print("  Version Compilation : ");
    Serial.println(s_versionCompilation);
    Serial.println("========================================");
}

#endif // COMPILE_DATE_HEURE_H
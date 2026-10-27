// CompileDateHeure.h
// Version : 1.1.1
// Auteur  : FOURNET Olivier
// Email   : olivier.fournet@free.fr
// GitHub  : https://github.com/Fo170/CompileDateHeure
// Date    : 2026-10-27
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
//       initVersionCompilation(FORMAT_DATE_NUMERIQUE);
//       Serial.println(s_versionCompilation);
//   }
//
// v1.1.1 (27/10/2026) : garde d'inclusion renforcée.
//   - #pragma once en plus du garde #ifndef COMPILE_DATE_HEURE_H
//   - les variables de portée globale sont déclarées `inline` (C++17) : inclure
//     ce header dans PLUSIEURS unités de compilation ne provoque plus d'erreur
//     « multiple definition » à l'édition de liens. Avec un standard antérieur
//     à C++17, la macro CDH_INLINE est vide et le comportement reste celui de
//     la v1.1.0 (une seule unité de compilation doit inclure le header).

#pragma once

#ifndef COMPILE_DATE_HEURE_H
#define COMPILE_DATE_HEURE_H


// Version de la librairie (source UNIQUE — agrégée par liste_lib.h)
#define __COMPILEDATEHEURE_H_VERSION__ "1.1.1"
#include <Arduino.h>
#include <stdint.h>

// ----- Liaison des variables de portée globale -----
// En C++17, `inline` autorise une DÉFINITION dans le header sans provoquer de
// « multiple definition » quand plusieurs unités de compilation l'incluent.
// Avant C++17 la macro est vide : le header reste utilisable, mais une seule
// unité de compilation doit l'inclure (comportement de la v1.1.0).
#if __cplusplus >= 201703L
  #define CDH_INLINE inline
#else
  #define CDH_INLINE
#endif

// ----- Données de compilation -----
CDH_INLINE const char* COMPILE_DATE = __DATE__;  // Format : "Mmm dd yyyy" (ex: "May 10 2026")
CDH_INLINE const char* COMPILE_TIME = __TIME__;  // Format : "hh:mm:ss"  (ex: "14:30:00")

// ----- N° de version -----
//#define COMPILE_DATE_HEURE_VERSION "1.0.1"

// ----- Constantes de format -----
#define FORMAT_DATE_NUMERIQUE     0  // "dd/mm/yyyy"
#define FORMAT_DATE_TEXTE_EN      1  // "dd/Mmm/yyyy" (anglais)
#define FORMAT_DATE_TEXTE_FR      2  // "dd/Mmm_fr/yyyy" (français)

// ----- Noms des mois (index 0 = Jan) -----
CDH_INLINE const char* MOIS_EN[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                                      "Jul","Aug","Sep","Oct","Nov","Dec"};
CDH_INLINE const char* MOIS_FR[12] = {"Jan","Fév","Mar","Avr","Mai","Jui",
                                      "Jui","Août","Sep","Oct","Nov","Déc"};

// ----- Chaîne de version globale -----
CDH_INLINE String s_versionCompilation;

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
inline String initVersionCompilation(int format = FORMAT_DATE_NUMERIQUE) {
    s_versionCompilation = getBuildDate(format) + "-" + String(COMPILE_TIME);
  return s_versionCompilation;
}

// ----- Afficher la date et l'heure de compilation sur Serial -----
// Utilisez getBuildDate() et initVersionCompilation() à la place
// Fonction supprimée à partir de la v1.0.1 — utiliser :
//   initVersionCompilation(FORMAT_DATE_NUMERIQUE);
//   Serial.println(s_versionCompilation);

// ----- NUM_VERSION : identifiant numérique de build -----
// Encodage  X.Y.Z  ->  X * 10^7 + Y * 10^4 + Z
//   X = année de compilation - NUM_VERSION_ANNEE_BASE   (0..429  -> 2020..2449)
//   Y = numéro du jour dans l'année                     (1..366)
//   Z = minute du jour                                  (0..1439)
// Exemple pour un build du 27/10/2026 à 18:13 :
//   X=6, Y=300, Z=1093  ->  s_numVersion = 63001093, s_numVersionTexte = "06.300.1093"
//
// Granularité : la minute. Deux compilations dans la même minute donnent la même valeur.
// Les secondes sont tronquées, jamais arrondies.
// Une date de compilation antérieure à NUM_VERSION_ANNEE_BASE donne 0.

#define NUM_VERSION_ANNEE_BASE   2020
#define NUM_VERSION_FORMAT_NUM   0  // valeur uint32_t
#define NUM_VERSION_FORMAT_TEXTE 1  // chaîne "XX.YYY.ZZZZ"

// ----- Chaînes/valeurs de NUM_VERSION (globales) -----
CDH_INLINE uint32_t s_numVersion = 0;        // 0 = non initialisé
CDH_INLINE String   s_numVersionTexte;       // vide = non initialisé

// ----- Jours par mois (index 0 = Jan) -----
const int JOURS_PAR_MOIS[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

// ----- Année bissextile (calendrier grégorien) -----
inline bool numVersionEstBissextile(int annee) {
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}

// ----- Décomposer la date/heure de compilation en (année, jour de l'année, minute du jour) -----
// Retourne false si l'année de compilation précède NUM_VERSION_ANNEE_BASE.
inline bool numVersionComposants(int &annee, int &jourAnnee, int &minuteJour) {
    // Extraction du jour (position 4, 1 ou 2 chiffres)
    int day = (COMPILE_DATE[4] == ' ')
              ? (COMPILE_DATE[5] - '0')
              : ((COMPILE_DATE[4] - '0') * 10 + (COMPILE_DATE[5] - '0'));

    // Recherche du mois par les 3 premiers caractères
    int monthIdx = 0;
    for (int i = 0; i < 12; i++) {
        if (COMPILE_DATE[0] == MOIS_EN[i][0] &&
            COMPILE_DATE[1] == MOIS_EN[i][1] &&
            COMPILE_DATE[2] == MOIS_EN[i][2]) {
            monthIdx = i;
            break;
        }
    }

    // Extraction de l'année (position 7-10)
    int year = (COMPILE_DATE[7] - '0') * 1000
             + (COMPILE_DATE[8] - '0') * 100
             + (COMPILE_DATE[9] - '0') * 10
             + (COMPILE_DATE[10] - '0');

    if (year < NUM_VERSION_ANNEE_BASE) {
        annee = 0; jourAnnee = 0; minuteJour = 0;
        return false;
    }

    // Numéro du jour dans l'année : somme des mois précédents + jour courant
    int doy = day;
    for (int m = 1; m <= monthIdx; m++) {
        doy += JOURS_PAR_MOIS[m - 1];
        if (m == 2 && numVersionEstBissextile(year)) doy += 1;
    }

    // Heure et minute depuis COMPILE_TIME ("hh:mm:ss")
    int hh = (COMPILE_TIME[0] - '0') * 10 + (COMPILE_TIME[1] - '0');
    int mn = (COMPILE_TIME[3] - '0') * 10 + (COMPILE_TIME[4] - '0');

    annee      = year - NUM_VERSION_ANNEE_BASE;
    jourAnnee  = doy;
    minuteJour = hh * 60 + mn;
    return true;
}

// ----- Obtenir NUM_VERSION sous forme numérique (ne touche pas les globales) -----
inline uint32_t getNumVersion() {
    int annee, jourAnnee, minuteJour;
    if (!numVersionComposants(annee, jourAnnee, minuteJour)) return 0UL;
    return (uint32_t)annee * 10000000UL
         + (uint32_t)jourAnnee * 10000UL
         + (uint32_t)minuteJour;
}

// ----- Obtenir NUM_VERSION sous forme texte "XX.YYY.ZZZZ" (ne touche pas les globales) -----
inline String getNumVersionTexte() {
    int annee, jourAnnee, minuteJour;
    if (!numVersionComposants(annee, jourAnnee, minuteJour)) return String("");

    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu.%03lu.%04lu",
             (unsigned long)annee,
             (unsigned long)jourAnnee,
             (unsigned long)minuteJour);
    return String(buf);
}

// ----- Initialiser NUM_VERSION (numérique + texte) et retourner la valeur numérique -----
inline uint32_t initNumVersion() {
    s_numVersion      = getNumVersion();
    s_numVersionTexte = getNumVersionTexte();
    return s_numVersion;
}

#endif // COMPILE_DATE_HEURE_H

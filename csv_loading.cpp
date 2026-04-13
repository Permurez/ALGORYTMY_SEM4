#include <stdlib.h>
#include <stdio.h>
#include "csv_loading.hpp"

#define MAKS_LINIA 2048

static void UtnijKoniecLinii(char* tekst)
{
    int i = 0;
    while (tekst[i] != '\0') {
        if (tekst[i] == '\r' || tekst[i] == '\n') {
            tekst[i] = '\0';
            return;
        }
        i++;
    }
}

static char* ZnajdzPierwszyZnak(char* tekst, char znak)
{
    int i = 0;
    while (tekst[i] != '\0') {
        if (tekst[i] == znak) {
            return &tekst[i];
        }
        i++;
    }
    return NULL;
}

static char* ZnajdzOstatniZnak(char* tekst, char znak)
{
    char* ostatni = NULL;
    int i = 0;
    while (tekst[i] != '\0') {
        if (tekst[i] == znak) {
            ostatni = &tekst[i];
        }
        i++;
    }
    return ostatni;
}

int LoadCSV(const char* nazwaPliku, MovieArray* tablicaFilmow) {
    FILE* plik = fopen(nazwaPliku, "r");
    if (!plik) {
        printf("BLAD , nie udalo sie otworzyc pliku %s\n", nazwaPliku);
        return 0;
    }

    char linia[MAKS_LINIA];
    int wczytaneRekordy = 0;

   
    if (fgets(linia, MAKS_LINIA, plik) == NULL) {
        fclose(plik);
        return 0;
    }

    while (fgets(linia, MAKS_LINIA, plik)) {
        // Usuwamy koniec linii, zeby latwiej dzielic rekord.
        UtnijKoniecLinii(linia);

        char* pierwszyPrzecinek = ZnajdzPierwszyZnak(linia, ',');
        if (!pierwszyPrzecinek) {
            continue;
        }
        char* poczatekTytulu = pierwszyPrzecinek + 1;


        char* ostatniPrzecinek = ZnajdzOstatniZnak(linia, ',');
        if (!ostatniPrzecinek || ostatniPrzecinek == pierwszyPrzecinek) {
            continue;
        }
        *ostatniPrzecinek = '\0';
        char* tekstOceny = ostatniPrzecinek + 1;

        // 4. Jesli tytul jest w cudzyslowie, obcinamy cudzyslowy z obu stron.
        if (*poczatekTytulu == '"') {
            poczatekTytulu++;
            char* zamykajacyCudzyslow = ZnajdzOstatniZnak(poczatekTytulu, '"');
            if (!zamykajacyCudzyslow) {
                continue;
            }
            *zamykajacyCudzyslow = '\0';
        }

        // 5. Parsowanie oceny; odrzucamy puste i nieliczbowe wartosci.
        char* koniecLiczby = NULL;
        float ocena = strtof(tekstOceny, &koniecLiczby);
        if (koniecLiczby == tekstOceny) {
            continue;
        }

        Push_back(tablicaFilmow, poczatekTytulu, ocena);
        wczytaneRekordy++;
    }

    fclose(plik);
    return wczytaneRekordy;
}
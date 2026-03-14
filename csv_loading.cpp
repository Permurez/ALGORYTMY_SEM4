#include <stdlib.h> 
#include <stdio.h>
#include "csv_loading.h"
#define MAX_LINE 2048 

int LoadCSV(const char* filename, MovieArray* arr) {
    // Otwieramy plik 
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("BLAD , nie udalo sie otworzyc pliku %s\n", filename);
        return 0;
    }

    char line[MAX_LINE];
    int loadedCount = 0;

    if (fgets(line, MAX_LINE, file) == NULL) {
        fclose(file);
        return 0; // Plik pusty
    }

    /
    while (fgets(line, MAX_LINE, file)) {
        char titleBuf[1024];  
        char ratingBuf[32];   
        
        int i = 0; 

        
        while (line[i] != ',' && line[i] != '\0') {
            i++;
        }
        if (line[i] == ',') i++; 
        
        int t = 0;
        bool inQuotes = false; //zmienna czy wewnatrza

        
        if (line[i] == '"') {
            inQuotes = true;
            i++; 
        }

        while (line[i] != '\0' && line[i] != '\n' && line[i] != '\r') {
            if (inQuotes) {
              
                if (line[i] == '"') {
                    inQuotes = false; 
                    i++;
                   
                    while (line[i] != ',' && line[i] != '\0' && line[i] != '\n') i++;
                    break;
                } else {
                    titleBuf[t++] = line[i++];
                }
            } else {
                
                if (line[i] == ',') {
                    break; 
                } else {
                    titleBuf[t++] = line[i++];
                }
            }
        }
        titleBuf[t] = '\0';
        if (line[i] == ',') i++; 

        // wczytywanie i sprawdzanie pustych
        int r = 0;
        bool isEmpty = true; 
        
        while (line[i] != '\0' && line[i] != '\n' && line[i] != '\r') {
            
            if (line[i] != ' ' && line[i] != ',') {
                isEmpty = false; 
            }
            ratingBuf[r++] = line[i++];
        }
        ratingBuf[r] = '\0'; 

        // Zapis do tablicy
        if (!isEmpty) {
            float rating = (float)atof(ratingBuf); 
            Push_back(arr, titleBuf, rating);
            loadedCount++;
        }
    }

    fclose(file); 
    return loadedCount;
}
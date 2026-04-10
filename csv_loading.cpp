#include <stdlib.h> 
#include <stdio.h>
#include "csv_loading.hpp"
#define MAX_LINE 2048 
#define MAX_TITLE 1024
#define MAX_RATING 32

int LoadCSV(const char* filename, MovieArray* arr) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("BLAD , nie udalo sie otworzyc pliku %s\n", filename);
        return 0;
    }

    char line[MAX_LINE];
    int loadedCount = 0;

    if (fgets(line, MAX_LINE, file) == NULL) {
        fclose(file);
        return 0;
    }

    while (fgets(line, MAX_LINE, file)) {
        char titleBuf[MAX_TITLE];
        char ratingBuf[MAX_RATING];
        int i = 0;
        int titleLength = 0;
        int ratingLength = 0;

        while (line[i] != '\0' && line[i] != ',' && line[i] != '\n' && line[i] != '\r')
            i++;
        if (line[i] == ',')
            i++;

        if (line[i] == '"') {
            i++;
            while (line[i] != '\0' && line[i] != '\n' && line[i] != '\r') {
                if (line[i] == '"') {
                    i++;
                    break;
                }
                if (titleLength < MAX_TITLE - 1)
                    titleBuf[titleLength++] = line[i];
                i++;
            }
            while (line[i] != '\0' && line[i] != ',' && line[i] != '\n' && line[i] != '\r')
                i++;
        } else {
            while (line[i] != '\0' && line[i] != ',' && line[i] != '\n' && line[i] != '\r') {
                if (titleLength < MAX_TITLE - 1)
                    titleBuf[titleLength++] = line[i];
                i++;
            }
        }
        titleBuf[titleLength] = '\0';
        if (line[i] == ',')
            i++;

        while (line[i] == ' ' || line[i] == '\t')
            i++;
        while (line[i] != '\0' && line[i] != ',' && line[i] != '\n' && line[i] != '\r') {
            if (ratingLength < MAX_RATING - 1)
                ratingBuf[ratingLength++] = line[i];
            i++;
        }
        while (ratingLength > 0 && (ratingBuf[ratingLength - 1] == ' ' || ratingBuf[ratingLength - 1] == '\t'))
            ratingLength--;
        ratingBuf[ratingLength] = '\0';

        if (ratingLength > 0) {
            float rating = (float)atof(ratingBuf); 
            Push_back(arr, titleBuf, rating);
            loadedCount++;
        }
    }

    fclose(file); 
    return loadedCount;
}
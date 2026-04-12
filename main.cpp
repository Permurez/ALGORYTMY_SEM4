#include "sorting.hpp"
#include "csv_loading.hpp"
#include <stdio.h>
#include <chrono>
// kalkulacja sredniej i mediany ocen w posortowanej tablicy filmow
float CalculateMean(const MovieArray* arr)
{
    double sum = 0;
    for (int i = 0; i < arr->size; i++)
        sum += arr->movies[i].rating;
    return (float)(sum / arr->size);
}

float CalculateMedian(const MovieArray* arr)
{
    int n = arr->size;
    if (n % 2 != 0)
        return arr->movies[n / 2].rating;
    return (arr->movies[(n - 1) / 2].rating + arr->movies[n / 2].rating) / 2.0f;
}

// funkcja pomocnicza do uruchomienia sortowania i pomiaru czasu
void RunRow(const char* algoName,void (*sortFunc)(MovieArray*, int, int)//wskaznik tablicy,left right
,const MovieArray* source,int n)//zrodlo danych, liczba rekordow do posortowania
{
    // Kopiujemy tylko pierwsze n rekordow ze zrodla
    MovieArray working;
    InitMovieArray(&working, n);
    for (int i = 0; i < n; i++)
        Push_back(&working, source->movies[i].title, source->movies[i].rating);

    auto start = std::chrono::steady_clock::now();
    sortFunc(&working, 0, working.size - 1);
    auto stop  = std::chrono::steady_clock::now();

    double ms   = std::chrono::duration<double, std::milli>(stop - start).count();
    float  mean = CalculateMean(&working);
    float  med  = CalculateMedian(&working);

    printf("| %-11s | %8d | %10.3f ms | %7.2f | %7.2f |\n",
           algoName, n, ms, mean, med);

    printf("   Sorted?: %s\n", IsSorted(&working) ? "OK" : "BLAD");

    FreeAllMovieArray(&working);
}

int main()
{
    MovieArray source;
    InitMovieArray(&source, 128);

    printf("Wczytywanie danych...\n");
    auto loadStart = std::chrono::steady_clock::now();
    int loaded = LoadCSV("projekt1_dane.csv", &source);
    auto loadStop = std::chrono::steady_clock::now();
    if (loaded <= 0) {
        printf("Blad: nie udalo sie wczytac pliku.\n");
        FreeAllMovieArray(&source);
        return 1;
    }
    printf("Wczytano %d rekordow.\n\n", loaded);

    double loadMs = std::chrono::duration<double, std::milli>(loadStop - loadStart).count();
    printf("Czas wczytywania i filtrowania pustych rankingow: %.3f ms\n\n", loadMs);

    int sizes[] = {10000, 100000, 500000, loaded};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < numSizes; i++) {
        int n = sizes[i];
        if (n > loaded)
            continue;

        bool duplicate = false;
        for (int j = 0; j < i; j++) {
            if (sizes[j] == n) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            continue;
        }

        printf("--- Test dla n=%d ---\n", n);

        printf("+-------------+----------+-------------+---------+---------+\n");
        printf("| Algorytm    | Rekordy  | Czas        | Srednia | Mediana |\n");
        printf("+-------------+----------+-------------+---------+---------+\n");
        RunRow("QuickSort",  QuickSort,  &source, n);
        RunRow("MergeSort",  MergeSort,  &source, n);
        RunRow("BucketSort", BucketSort, &source, n);
        printf("+-------------+----------+-------------+---------+---------+\n");
        printf("\n");
      
    }

    // Najgorszy przypadek BucketSort = wszystkie elementy trafiaja do jednego kubelka
    printf("\n--- Najgorszy przypadek BucketSort (wszystkie oceny = 2.0) ---\n");
    
    int sizesForBucketWorst[] = {10000, 100000, 500000, loaded};
    int numBucketWorstSizes = sizeof(sizesForBucketWorst) / sizeof(sizesForBucketWorst[0]);
    
    for (int i = 0; i < numBucketWorstSizes; i++) {
        int n = sizesForBucketWorst[i];
        if (n > loaded)
            continue;
        
        MovieArray bucketWorst;
        InitMovieArray(&bucketWorst, n);
        for (int j = 0; j < n; j++)
            Push_back(&bucketWorst, source.movies[j].title, 2.0f);
        
        printf("+-------------+----------+-------------+---------+---------+\n");
        printf("| Algorytm    | Rekordy  | Czas        | Srednia | Mediana |\n");
        printf("+-------------+----------+-------------+---------+---------+\n");
        RunRow("BucketSort(worst)", BucketSort, &bucketWorst, n);
        printf("+-------------+----------+-------------+---------+---------+\n");
        
        FreeAllMovieArray(&bucketWorst);
    }

    // Najgorszy przypadek QuickSort = dane juz posortowane
    // Limit rozmiarow chroni przed stack overflow przy skrajnie zlym pivocie.
    printf("\n--- Najgorszy przypadek QuickSort (dane posortowane) ---\n");
    MovieArray sorted;
    CopyMovieArray(&source, &sorted);
    MergeSort(&sorted, 0, sorted.size - 1); // sortujemy zeby miec posortowane dane
    
    int sizesForWorstQuickSort[] = {5000, 10000};
    int numWorstSizes = sizeof(sizesForWorstQuickSort) / sizeof(sizesForWorstQuickSort[0]);
    
    for (int i = 0; i < numWorstSizes; i++) {
        int n = sizesForWorstQuickSort[i];
        if (n > sorted.size)
            continue;
        
        printf("+-------------+----------+-------------+---------+---------+\n");
        printf("| Algorytm    | Rekordy  | Czas        | Srednia | Mediana |\n");
        printf("+-------------+----------+-------------+---------+---------+\n");
        RunRow("QuickSort(worst)", QuickSortWorst, &sorted, n);
        printf("+-------------+----------+-------------+---------+---------+\n");
    }
    
    FreeAllMovieArray(&sorted);

    FreeAllMovieArray(&source);
    printf("Nacisnij Enter aby zamknac...");
    getchar();
    return 0;
}
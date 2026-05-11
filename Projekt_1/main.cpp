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
void RunRow(const char* algoName, void (*sortFunc)(MovieArray*, int, int),
            const MovieArray* source, int n)
{
    MovieArray working;
    InitMovieArray(&working, n);
    for (int i = 0; i < n; i++) {
        Push_back(&working, source->movies[i].title, source->movies[i].rating);
    }

    const auto start = std::chrono::steady_clock::now();
    sortFunc(&working, 0, n - 1);
    const double elapsedMs = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - start).count();

    const float mean = CalculateMean(&working);
    const float median = CalculateMedian(&working);
    const bool sorted = IsSorted(&working);

    printf("| %-11s | %8d | %10.3f ms | %7.2f | %7.2f |\n",
           algoName, n, elapsedMs, mean, median);

    printf("   Sorted?: %s\n", sorted ? "OK" : "BLAD");

    FreeAllMovieArray(&working);
}

void PrintTableHeader()
{
    printf("+-------------+----------+-------------+---------+---------+\n");
    printf("| Algorytm    | Rekordy  | Czas        | Srednia | Mediana |\n");
    printf("+-------------+----------+-------------+---------+---------+\n");
}

void PrintTableFooter()
{
    printf("+-------------+----------+-------------+---------+---------+\n");
}

// Czysci pozostale znaki po scanf z bufora wejscia.
void ClearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}


int AskRecordsCount(int loaded)
{
    int minCount = 10;
    int maxCount = 1000;

    if (loaded < minCount) { minCount = loaded;}
    if (loaded < maxCount) { maxCount = loaded;}

    if (loaded <= 0) {return 0;}

    int n = minCount;
    while (true) {
        printf("Ile rekordow posortowac i pokazac? (%d-%d): ", minCount, maxCount);
        if (scanf("%d", &n) != 1) {n = minCount;}
        ClearInputBuffer();

        if (n >= minCount && n <= maxCount) {return n;}

        printf("Niepoprawna liczba. Sprobuj ponownie.\n");
    }
}

// Odczyt wyboru algorytmu od uzytkownika; przy blednym wpisie domyslnie wybieramy QuickSort.
int AskSortChoice()
{
    int wybor = 1;
    printf("\n\nKtorym algorytmem sortujemy? (1-Quick, 2-Merge, 3-Bucket): ");
    if (scanf("%d", &wybor) != 1) {
        wybor = 1;
    }
    ClearInputBuffer();

    return wybor;
}

const char* SortNameByChoice(int wybor)
{
    switch (wybor) {
    case 1:
        return "QuickSort";
    case 2:
        return "MergeSort";
    case 3:
        return "BucketSort";
    default:
        return "QuickSort";
    }
}

// Uruchamia wybrany algorytm na calej przekazanej tablicy.
void SortWithChoice(MovieArray* arr, int wybor)
{
    switch (wybor) {
    case 1:
        QuickSort(arr, 0, arr->size - 1);
        break;
    case 2:
        MergeSort(arr, 0, arr->size - 1);
        break;
    case 3:
        BucketSort(arr, 0, arr->size - 1);
        break;
    default:
        QuickSort(arr, 0, arr->size - 1);
        break;
    }
}


void RunDemo(const MovieArray* source, int loaded)
{
    int demoSize = AskRecordsCount(loaded);

    MovieArray demo;
    InitMovieArray(&demo, demoSize);
    for (int i = 0; i < demoSize; i++) {
        Push_back(&demo, source->movies[i].title, source->movies[i].rating);
    }

    printf("\n--- DEMO NA %d REKORDACH ---\n", demoSize);

    printf("Kolejnosc PRZED sortowaniem:\n");
    for (int i = 0; i < demoSize; i++) {
        printf("%3d. %.1f | %s\n", i + 1, demo.movies[i].rating, demo.movies[i].title);
    }

    int wybor = AskSortChoice();
    const auto demoSortStart = std::chrono::steady_clock::now();
    SortWithChoice(&demo, wybor);
    const double demoSortMs = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - demoSortStart).count();

    printf("Wynik po sortowaniu (%s):\n", SortNameByChoice(wybor));
    for (int i = 0; i < demoSize; i++) {
        printf("%3d. %.1f | %s\n", i + 1, demo.movies[i].rating, demo.movies[i].title);
    }
    printf("Czas sortowania: %.3f ms\n", demoSortMs);
    printf("\nStatus: %s\n", IsSorted(&demo) ? "POSORTOWANO" : "NIE POSORTOWANO");

    FreeAllMovieArray(&demo);
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

    // 1) Pokaz dzialania na malej probce.
    RunDemo(&source, loaded);

    // 2) Porownanie standardowych przypadkow.
    printf("--- Test dla n=%d ---\n", 10000);
    PrintTableHeader();
    RunRow("QuickSort", QuickSort, &source, 10000);
    RunRow("MergeSort", MergeSort, &source, 10000);
    RunRow("BucketSort", BucketSort, &source, 10000);
    PrintTableFooter();
    printf("\n");

    printf("--- Test dla n=%d ---\n", 100000);
    PrintTableHeader();
    RunRow("QuickSort", QuickSort, &source, 100000);
    RunRow("MergeSort", MergeSort, &source, 100000);
    RunRow("BucketSort", BucketSort, &source, 100000);
    PrintTableFooter();
    printf("\n");

    printf("--- Test dla n=%d ---\n", 500000);
    PrintTableHeader();
    RunRow("QuickSort", QuickSort, &source, 500000);
    RunRow("MergeSort", MergeSort, &source, 500000);
    RunRow("BucketSort", BucketSort, &source, 500000);
    PrintTableFooter();
    printf("\n");

    printf("--- Test dla n=%d ---\n", loaded);
    PrintTableHeader();
    RunRow("QuickSort", QuickSort, &source, loaded);
    RunRow("MergeSort", MergeSort, &source, loaded);
    RunRow("BucketSort", BucketSort, &source, loaded);
    PrintTableFooter();
    printf("\n");

    // 3) Najgorszy przypadek BucketSort: wszystkie oceny takie same.
    printf("\n--- Najgorszy przypadek BucketSort (wszystkie oceny = 2.0) ---\n");

    MovieArray bucketWorst10000;
    InitMovieArray(&bucketWorst10000, 10000);
    for (int j = 0; j < 10000; j++) {
        Push_back(&bucketWorst10000, source.movies[j].title, 2.0f);
    }
    PrintTableHeader();
    RunRow("BucketSort(worst)", BucketSort, &bucketWorst10000, 10000);
    PrintTableFooter();
    FreeAllMovieArray(&bucketWorst10000);

    MovieArray bucketWorst100000;
    InitMovieArray(&bucketWorst100000, 100000);
    for (int j = 0; j < 100000; j++) {
        Push_back(&bucketWorst100000, source.movies[j].title, 2.0f);
    }
    PrintTableHeader();
    RunRow("BucketSort(worst)", BucketSort, &bucketWorst100000, 100000);
    PrintTableFooter();
    FreeAllMovieArray(&bucketWorst100000);

    MovieArray bucketWorst500000;
    InitMovieArray(&bucketWorst500000, 500000);
    for (int j = 0; j < 500000; j++) {
        Push_back(&bucketWorst500000, source.movies[j].title, 2.0f);
    }
    PrintTableHeader();
    RunRow("BucketSort(worst)", BucketSort, &bucketWorst500000, 500000);
    PrintTableFooter();
    FreeAllMovieArray(&bucketWorst500000);

    MovieArray bucketWorstLoaded;
    InitMovieArray(&bucketWorstLoaded, loaded);
    for (int j = 0; j < loaded; j++) {
        Push_back(&bucketWorstLoaded, source.movies[j].title, 2.0f);
    }
    PrintTableHeader();
    RunRow("BucketSort(worst)", BucketSort, &bucketWorstLoaded, loaded);
    PrintTableFooter();
    FreeAllMovieArray(&bucketWorstLoaded);

    // 4) Najgorszy przypadek QuickSort: dane juz posortowane.
    printf("\n--- Najgorszy przypadek QuickSort (dane posortowane) ---\n");
    MovieArray sorted;
    CopyMovieArray(&source, &sorted);
    MergeSort(&sorted, 0, sorted.size - 1);

    PrintTableHeader();
    RunRow("QuickSort(worst)", QuickSortWorst, &sorted, 5000);
    PrintTableFooter();

    PrintTableHeader();
    RunRow("QuickSort(worst)", QuickSortWorst, &sorted, 10000);
    PrintTableFooter();

    PrintTableHeader();
    RunRow("QuickSort(worst)", QuickSortWorst, &sorted, 20000);
    PrintTableFooter();

    PrintTableHeader();
    RunRow("QuickSort(worst)", QuickSortWorst, &sorted, 50000);
    PrintTableFooter();

    PrintTableHeader();
    RunRow("QuickSort(worst)", QuickSortWorst, &sorted, sorted.size);
    PrintTableFooter();

    FreeAllMovieArray(&sorted);

    FreeAllMovieArray(&source);
    printf("Nacisnij Enter aby zamknac...");
    getchar();
    return 0;
}
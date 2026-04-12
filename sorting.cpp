#include "sorting.hpp"
#include <stdlib.h>

void Swap(Movie *a, Movie *b)//zmiana miejscami dwoch elementow
{
    Movie temp = *a;
    *a = *b;
    *b = temp;
}

int Partition(MovieArray *arr, int left, int right)
{
    int mid = left + (right - left) / 2;
    float pivot = arr->movies[mid].rating;
    Swap(&arr->movies[mid], &arr->movies[right]);//pivot na koniec
    int i = left - 1;
    for (int j = left; j < right; j++)
    {
        if (arr->movies[j].rating <= pivot)
        {
            i++;
            Swap(&arr->movies[i], &arr->movies[j]);
        }
    } //wstawianie wlasciwego pivota na swoje miejsce
    Swap(&arr->movies[i + 1], &arr->movies[right]);
    return i + 1; //index pivota
}

int PartitionForWorstQuickSort(MovieArray *arr, int left, int right)
{
    // Osobna wersja tylko do testu najgorszego przypadku QuickSort.
    // Bierzemy pierwszy element jako pivot, zeby dla danych juz posortowanych
    // QuickSort mial faktycznie najgorszy przypadek.
    float pivot = arr->movies[left].rating;
    Swap(&arr->movies[left], &arr->movies[right]);//pivot na koniec
    int i = left - 1;
    for (int j = left; j < right; j++)
    {
        if (arr->movies[j].rating <= pivot)
        {
            i++;
            Swap(&arr->movies[i], &arr->movies[j]);
        }
    }
    Swap(&arr->movies[i + 1], &arr->movies[right]);
    return i + 1;
}

void Merge(MovieArray *arr, int left, int mid, int right)
{
    int lenLeft = mid - left + 1;
    int lenRight = right - mid;

    Movie* tmpLeft = new Movie[lenLeft];
    Movie* tmpRight = new Movie[lenRight];
    for (int i = 0; i < lenLeft; i++)
    {
        tmpLeft[i] = arr->movies[left + i];
    }
    for (int j = 0; j < lenRight; j++)
    {
        tmpRight[j] = arr->movies[mid + 1 + j];
    }
    int i = 0;
    int j = 0;
    int k = left;
    while (i < lenLeft && j < lenRight)
    {
        if (tmpLeft[i].rating <= tmpRight[j].rating)
            arr->movies[k++] = tmpLeft[i++];
        else
            arr->movies[k++] = tmpRight[j++];
    }
    while (i < lenLeft) arr->movies[k++] = tmpLeft[i++];
    while (j < lenRight)arr->movies[k++] = tmpRight[j++];
    delete[] tmpLeft;
    delete[] tmpRight;
}

void MergeSort(MovieArray *arr, int left, int right)
{
    if (left >= right)
        return; // 1 element — koniec

    int mid = left + (right - left) / 2;

    MergeSort(arr, left, mid);
    MergeSort(arr, mid + 1, right);
    Merge(arr, left, mid, right);
}

void InsertionSort(MovieArray* arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int j = i; //dopoki element po  lewej jest wiekszy od obecnego zamin miejscami
        while (j > left && arr->movies[j - 1].rating > arr->movies[j].rating) {
            Swap(&arr->movies[j - 1], &arr->movies[j]);
            j--; //w lewo
        }
    }
}

void QuickSort(MovieArray* arr, int left, int right)
{
    // petla zastępuje drugą rekurencję — nie tworzy nowej ramki stosu 
    while (left < right) //to nie powoduje stack overflow 
    {
        int pivotIndex = Partition(arr, left, right);

        if (pivotIndex - left < right - pivotIndex)
        {
            // lewa czesc jest mniejsza — schodzimy w nia rekurencyjnie
            QuickSort(arr, left, pivotIndex - 1);
            left = pivotIndex + 1;
        }
        else
        {
            QuickSort(arr, pivotIndex + 1, right);
            right = pivotIndex - 1;
        }
    }
    // rekurencja zawsze idzie w czesc mniejsza, wiec stos sie nie przepelni
}

void QuickSortWorst(MovieArray* arr, int left, int right)
{
    if (left >= right)
        return;

    int pivotIndex = PartitionForWorstQuickSort(arr, left, right);
    QuickSortWorst(arr, left, pivotIndex - 1);
    QuickSortWorst(arr, pivotIndex + 1, right);
}

void BucketSort(MovieArray *arr, int left, int right) {
    int numMoviesToSort = right - left + 1; //ile elementow do posortowania
    if (numMoviesToSort <= 1) return;

    // 1. Tworzymy 11 wiader (zakres ocen 0-10)
    MovieArray buckets[11];
    for (int i = 0; i < 11; i++) {
        // Inicjalizacja z przewidywanym rozmiarem (numMoviesToSort / 10) // dodatkowo 10 aby unikac zbyt czestych realokacji
        InitMovieArray(&buckets[i], (numMoviesToSort / 10) +10); //
    }

    // 2. Rozdzielanie do wiader
    for (int i = left; i <= right; i++) {
        int idx = (int)arr->movies[i].rating; // tutaj ucinamy czesc dziesietna rzutowaniem
        if (idx < 0) idx = 0; 
        if (idx > 10) idx = 10;
        
        // Przenosimy dane do odpowiedniego kubełka
        Push_back(&buckets[idx], arr->movies[i].title, arr->movies[i].rating);//kopiujemy tytul i ocene do odpowiedniego kubla
    }//kazdy trzyma 0-1, 1-2, 10 trzyma tylko 10


    int currentPos = left; 
    for (int i = 0; i < 11; i++) { // petla po wszystkich kublach
        if (buckets[i].size > 0) {//czy cokolwiek jest w kuble
    
            InsertionSort(&buckets[i], 0, buckets[i].size - 1);//sortowanie kazdego kubla z osobna insertion sortem 
            for (int j = 0; j < buckets[i].size; j++) {
                delete[] arr->movies[currentPos].title;
                // Przypisanie wskaznikow z kubla do oryginalnej tablicy
                arr->movies[currentPos].title = buckets[i].movies[j].title; 
                arr->movies[currentPos].rating = buckets[i].movies[j].rating;
                buckets[i].movies[j].title = nullptr;
                currentPos++;
            }
        }
        FreeAllMovieArray(&buckets[i]); // Sprzątanie struktury kubełka
    }
}
// sprawdzenie czy tablica jest posortowana rosnaco (lewo=min prawo=max)
bool IsSorted(const MovieArray* arr) {
    for (int i = 0; i < arr->size - 1; i++)
        if (arr->movies[i].rating > arr->movies[i + 1].rating)
            return false;
    return true;
}

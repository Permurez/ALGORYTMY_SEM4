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
void QuickSort(MovieArray *arr, int left, int right)
{
    if (left >=right) return;
    int pivotIndex = Partition(arr, left, right);
    QuickSort(arr, left, pivotIndex - 1); // rekurencja po obu stronach pivota
    QuickSort(arr, pivotIndex + 1, right);
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
    while (i < lenLeft)
        arr->movies[k++] = tmpLeft[i++];
    while (j < lenRight)
        arr->movies[k++] = tmpRight[j++];
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
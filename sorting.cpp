#include "sorting.hpp"
#include <stdlib.h>

void Merge(MovieArray *arr, int left, int mid, int right)
{
    int lenLeft = mid - left + 1;
    int lenRight = right - mid;
    Movie *tmpLeft = (Movie *)malloc(lenLeft * sizeof(Movie));
    Movie *tmpRight = (Movie *)malloc(lenRight * sizeof(Movie)); // rezerwacja dla polowek
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
    free(tmpLeft);
    free(tmpRight);
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
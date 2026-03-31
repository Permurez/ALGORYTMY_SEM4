#ifndef SORTING_HPP
#define SORTING_HPP

#include "data_structures.hpp"
//pomocnicze funkcje do sortowania
void Swap(Movie* a, Movie* b);
int Partition(MovieArray* arr, int left, int right);
void InsertionSort(MovieArray* arr, int left, int right);
//glowne funkcje sortowania
void MergeSort(MovieArray* arr, int left, int right);
void QuickSort(MovieArray* arr, int left, int right);
void BucketSort(MovieArray* arr, int left, int right);

#endif

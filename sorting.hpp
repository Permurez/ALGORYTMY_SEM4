#ifndef SORTING_HPP
#define SORTING_HPP

#include "data_structures.hpp"
//pomocnicze funkcje do sortowania
void Swap(Movie* a, Movie* b);
int Partition(MovieArray* arr, int left, int right);
int PartitionForWorstQuickSort(MovieArray* arr, int left, int right);
void InsertionSort(MovieArray* arr, int left, int right);//sorotowanie kublow bo dla malychrozmiarow jest szybki
//glowne funkcje sortowania
void MergeSort(MovieArray* arr, int left, int right);
void QuickSort(MovieArray* arr, int left, int right);
void QuickSortWorst(MovieArray* arr, int left, int right);
void BucketSort(MovieArray* arr, int left, int right);

bool IsSorted(const MovieArray* arr);
#endif

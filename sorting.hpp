#ifndef SORTING_HPP
#define SORTING_HPP

#include "data_structures.hpp"
void Swap(Movie* a, Movie* b);
int Partition(MovieArray* arr, int left, int right);
void MergeSort(MovieArray* arr, int left, int right);
void QuickSort(MovieArray* arr, int left, int right);
void IntroSort(MovieArray* arr, int left, int right);

#endif

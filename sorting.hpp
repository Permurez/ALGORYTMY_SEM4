#ifndef SORTING_HPP
#define SORTING_HPP

#include "data_structures.hpp"

void MergeSort(MovieArray* arr, int left, int right);
void QuickSort(MovieArray* arr, int left, int right);
void IntroSort(MovieArray* arr, int left, int right);
void BucketSort(MovieArray* arr);//nie jest rekurencyjny 

#endif

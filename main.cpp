
#include "sorting.hpp"
#include <stdio.h>

int main() {
    MovieArray arr;
    InitMovieArray(&arr, 10);
    Push_back(&arr, "Film C", 7.5f);
    Push_back(&arr, "Film A", 3.2f);
    Push_back(&arr, "Film B", 9.1f);
    Push_back(&arr, "Film D", 1.0f);

    QuickSort(&arr, 0, arr.size - 1);

    for (int i = 0; i < arr.size; i++)
        printf("%.1f %s\n", arr.movies[i].rating, arr.movies[i].title);

    FreeAllMovieArray(&arr);
    return 0;
}

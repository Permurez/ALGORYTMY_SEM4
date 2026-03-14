#include "data_structures.hpp"  
#include <stdlib.h>
#include <string.h>
void InitMovieArray(MovieArray *arr, int initialCapacity)
{
 Movie *allocation =(Movie*)malloc(initialCapacity* sizeof(Movie));   
    arr->movies= allocation;
    arr->size=0;
    arr->capacity= initialCapacity;
}//rezerwacja miesca dla tablicy filmow, uznanie ze pusto i miejsce na start z initialCapacity

void Push_back(MovieArray *arr, const char *title, float rating)
{
    if (arr->size == arr->capacity){
     arr->capacity *= 2;
     arr-> movies= (Movie*)realloc(arr->movies, arr->capacity* sizeof(Movie));
    }
    arr->movies[arr->size].title = strdup(title);
    arr->movies[arr->size].rating = rating;
    arr->size++;
}
void FreeAllMovieArray(MovieArray *arr)
{
    for (int i=0; i<arr->size; i++){
        free(arr->movies[i].title);//zwalniam wzkaznik tytulu
    }
    free(arr->movies);
}
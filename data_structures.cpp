#include "data_structures.hpp"  
#include <stdlib.h>
#include <string.h>
#include <iostream>
void InitMovieArray(MovieArray *arr, int initialCapacity)
{
    arr->movies = new Movie[initialCapacity];  
    arr->size=0;
    arr->capacity= initialCapacity;
}//rezerwacja miesca dla tablicy filmow, uznanie ze pusto i miejsce na start z initialCapacity

void Push_back(MovieArray *arr, const char *title, float rating)  
{
if (arr->size == arr->capacity){//sprawdzam czy jest miejsce,JNT*2
    arr->capacity *= 2;
    
    //Movie *tmp = (Movie*)realloc(arr->movies, arr->capacity* sizeof(Movie));
    // if (tmp == nullptr) {std::cout << "BLAD: brak pamieci\n";return;}
    Movie *tmp = new Movie[arr->capacity];
    for (int i = 0; i < arr->size; i++) {
        tmp[i] = arr->movies[i];
    }
 delete[] arr->movies;
arr->movies = tmp;
}
    arr->movies[arr->size].title = new char[strlen(title) + 1]; //alokacja pamieci dla tytulu
    strcpy(arr->movies[arr->size].title, title);
    arr->movies[arr->size].rating = rating;//kopiuje tytul i ocene do tablicy filmow
    arr->size++;
}
void FreeAllMovieArray(MovieArray *arr)//zwalniam wzkaznik tytulu dla kazdego filmu 
{
    for (int i=0; i<arr->size; i++){
        delete[] arr->movies[i].title;
    }//a potem zwalniam pamiec dla tablicy filmow
    delete[] arr->movies;
}
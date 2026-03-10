#pragma oce

struct Movie {
    char* title; //wskaznika na pierwszy znak tytulu
    float rating;
};

struct MovieArray {
    Movie* movies;
    int size;
    int capacity;
};
//inicjalizacja tablicy nadajac pojemnosc
void InitMovieArray(MovieArray* arr, int initialCapacity);

void Push_back(MovieArray* arr, const char* title, float rating);

void FreeAllMovieArray(MovieArray* arr);
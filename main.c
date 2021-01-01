#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dataset.h"


// clean this mess
char** init_bag_of_word(struct Movie* movie1, struct Movie* movie2, int* counter, int* counter_genres){
    *counter = movie1->nb_genres;
    int counter_nconsts = movie1->nb_nconst;
    char** list = malloc(sizeof(char *) * (*counter));

    for (int i = 0; i < *counter; i++) {
        (*counter_genres)++;
        list[i] = movie1->genres[i];
    }

    for (int i = 0; i < movie2->nb_genres; ++i){
        int counter2 = 0;
        char* str1 = movie2->genres[i];
        for (int j = 0; j < *counter; ++j) {
            char* str2 = movie1->genres[j];
            if (strcmp(str1, str2) == 0){
                counter2++;
                break;
            }
        }

        if (counter2 == 0){
            (*counter)++;
            (*counter_genres)++;
            list = realloc(list, sizeof(char *) * *counter);
            list[(*counter)-1] = movie2->genres[i];
        }
    }

    for (int i = 0; i < counter_nconsts; ++i) {
        (*counter)++;
        list = realloc(list, sizeof(char *) * (*counter));
        list[(*counter)-1] = movie1->nconsts[i];
    }


    for (int i = 0; i < movie2->nb_nconst; ++i){
        int counter2 = 0;
        char* str1 = movie2->nconsts[i];
        for (int j = 0; j < counter_nconsts; ++j) {
            char* str2 = movie1->nconsts[j];
            if (strcmp(str1, str2) == 0){
                counter2++;
                break;
            }
        }

        if (counter2 == 0){
            (*counter)++;
            list = realloc(list, sizeof(char *) * (*counter));
            list[(*counter)-1] = movie2->nconsts[i];
        }
    }
    return list;
}


// inverse document frequency
double idf_nconsts(char* word, struct Movie* dataset){
    int counter = 0;
    double result;

    for (int i = 0; i < 246710; ++i) {
        struct Movie movie = dataset[i];
        for (int j = 0; j < movie.nb_nconst; ++j) {
            if (strcmp(word, movie.nconsts[j]) == 0){
                counter++;
            }
        }
    }

    result = log10(246710.0/counter);

    return result;
}

double idf_genres(char* word, struct Movie* dataset){
    int counter = 0;
    double result;

    for (int i = 0; i < 246710; ++i) {
        struct Movie movie = dataset[i];
        for (int j = 0; j < movie.nb_genres; ++j) {
            if (strcmp(word, movie.genres[j]) == 0){
                counter++;
            }
        }
    }

    result = log10(246710.0/counter);

    return result;
}


double* init_list_idf(char** list, int counter, int counter_genres, struct Movie* dataset){
    double* list_idf = malloc(sizeof(double) * (counter));
    for (int i = 0; i < counter; ++i) {
        if (i < counter_genres){
            list_idf[i] = idf_genres(list[i], dataset);
        } else {
            list_idf[i] = idf_nconsts(list[i], dataset);
        }
    }

    return list_idf;
}

void init_vector(double* vector, struct Movie* movie, int counter, int counter_genres, char** list, double* list_idf){
    for (int i = 0; i < counter; ++i) {
        char* str1 = list[i];
        if (i < counter_genres){
            for (int j = 0; j < movie->nb_genres; j++){
                char* str2 = movie->genres[j];
                if (strcmp(str1, str2) == 0){
                    vector[i] = list_idf[i];
                    break;
                } else {
                    vector[i] = 0.0;
                }

            }
        } else {
            for (int j = 0; j < movie->nb_nconst; j++){
                char* str2 = movie->nconsts[j];
                if (strcmp(str1, str2) == 0){
                    vector[i] = list_idf[i];
                    break;
                } else {
                    vector[i] = 0.0;
                }

            }
        }

    }
}


double get_cosine_similarity(struct Movie* movie1, struct Movie* movie2, struct Movie* dataset){
    int counter = 0;
    int counter_genres = 0;
    char** list = init_bag_of_word(movie1, movie2, &counter, &counter_genres);
    double* list_idf = init_list_idf(list, counter, counter_genres, dataset);
    double* vector1 = malloc(sizeof(double) * counter);
    double* vector2 = malloc(sizeof(double) * counter);

    init_vector(vector1, movie1, counter, counter_genres, list, list_idf);
    init_vector(vector2, movie2, counter, counter_genres, list, list_idf);


    //todo: cosine similarity


    free(vector1);
    free(vector2);
    free(list);
    free(list_idf);
    return 0.0;
}


// main
int main() {
    struct Movie *dataset; // the list of movies and their details
    double cosine;


    // get the dataset from local file into a list of Movies (made be commit suicide)
    FILE* dataset_file = fopen("data.tsv", "r"); // dataset local .tsv file
    if (dataset_file == NULL){
        printf("\nCould not read dataset file.\nExit...");
        exit(EXIT_FAILURE);
    }
    dataset = init_dataset(dataset_file);
    fclose(dataset_file);

    /*for (int i = 0; i < 10; ++i) {
        printf("\n%d", dataset[i].nb_genres);
    }*/


    cosine = get_cosine_similarity(&dataset[0], &dataset[1], dataset);


    free_dataset(dataset);
    //free(dataset); //cannot free because heap corruption (wtf) (maybe I forgot to free something inside idk)
    return 0;
}

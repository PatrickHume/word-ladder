#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "global.c"

// the length of the word
const int wordLength = 5;

// return true if two five letter words share all but 1 characters
int isNeighbour(char* a, char* b){
    bool found = false;
    bool neighbour = true;
    for(int i = 0; i < wordLength; i++){
        neighbour = !(found && (bool)(a[i] != b[i])) && neighbour;
        found = found ^ (bool)(a[i] != b[i]) && neighbour;
    }
    return found;
}

// the heuristic, the minimum distance between two words
int h(char* a, char* b){
    int score = 0;
    for(int i = 0; i < wordLength; i++){
        score += (int)(a[i] != b[i]);
    }
    return score;
}
// the length of the dictionary
const unsigned int dictLength = 5757;

// maps two dictionary indicies to one index in the precalculated distances array
int m(int x, int y){
    return (y*dictLength) + x;
}

// finds the longest shortest word ladder
int main(int argc, char *argv[])
{
    // an array containing all words from the text file
    char (*allWords)[6] = malloc(sizeof(allWords)*dictLength);
    check(allWords);
    FILE* file = fopen("allWords.txt", "r");
    // read each word into the array
    char line[7];
    int lineNumber = 0;
    while (fgets(line, sizeof(line), file)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        line[strcspn(line, "\n")] = 0;
        strcpy(allWords[lineNumber++],line);
    }
    fclose(file);

    // get the length of distance pairs
    int trueLength = dictLength*dictLength;
    // array to hold precalculated distance pairs
    unsigned char* allDistances = malloc(sizeof(unsigned char)*trueLength);
    check(allDistances);
    // populate array with distance pairs
    for(int y = 0; y < dictLength; y++){
        // populate with exact matches and neighbours
        for(int x = 0; x < dictLength; x++){
            if(x == y){
                allDistances[m(x,y)] = 0;
            }else if(isNeighbour(allWords[x],allWords[y])){
                allDistances[m(x,y)] = 1;
            }else{
                allDistances[m(x,y)] = (unsigned char)255;
            }
        }
    }

    

    /* here, do your time-consuming job */
    clock_t begin = clock();
    clock_t end;

    // begin calculating longer distances
    for(int k = 0; k < dictLength; k++){
        // update user on progress
        if(k % (dictLength/20) == 0){
            end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("%i%% complete - time elapsed: %.1f seconds\n",(int)((k*100)/dictLength), time_spent);
            begin = clock();
        }
        // for each pair of words
        bool midpointKnown;
        bool providesShortCut;
        for(int i = 0; i < dictLength; i++){
            for(int j = 0; j < dictLength; j++){
                midpointKnown = (!(allDistances[m(i,j)] == 0 || allDistances[m(i,j)] == 1 || allDistances[m(i,k)] == (unsigned char)255 || allDistances[m(k,j)] == (unsigned char)255));
                providesShortCut = (allDistances[m(i,j)] > allDistances[m(i,k)] + allDistances[m(k,j)]);
                allDistances[m(i,j)] = midpointKnown && providesShortCut ? allDistances[m(i,k)] + allDistances[m(k,j)] : allDistances[m(i,j)];
            }
        } 
    }
    // begin searching for the longest shortest path
    int greatestDist, greatestX, greatestY;
    greatestDist = 0;
    greatestX = 0;
    greatestY = 0;
    // for each pair of values check the distance
    for(int y = 0; y < dictLength; y++){
        for(int x = 0; x < dictLength; x++){
            // skip unknown distances
            if(allDistances[m(x,y)] == (unsigned char)255){
                continue;
            }
            // if a greater distance is found, update the max dist
            if(allDistances[m(x,y)] > greatestDist){
                greatestDist = allDistances[m(x,y)];
                greatestX = x;
                greatestY = y;
            }
            printf("%s -> %s dist: %i\n",allWords[x],allWords[y],allDistances[m(x,y)]);
        }
    }
    // display the words furthest apart
    printf("dist:%i %s %s\n",greatestDist,allWords[greatestX],allWords[greatestY]);
    // free the memory
    free(allWords);
    free(allDistances);
    return 0;
}
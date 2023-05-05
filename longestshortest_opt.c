#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <sys/resource.h>
#include "global.c"
// the length of the word
const int wordLength = 5;
// the length of the word with a newline
const int wordLengthSpace = wordLength+1;

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

int run(){

    char allWords[6000][wordLengthSpace];
    int allWordsLength = 0;
    // open dictionary file and write all words to the hashset
    FILE* file = fopen("allWords.txt", "r");
    char line[wordLengthSpace+1];
    int counter = 0;
    const int maxWords = 5757;
    while (fgets(line, sizeof(line), file)) {
        if(counter++ >= maxWords){
            break;
        }
        /* note that fgets doesn't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        line[strcspn(line, "\n")] = 0;
        strncpy(allWords[allWordsLength++], line, wordLengthSpace);
    }
    // close the dictionary file
    fclose(file);
    
    struct rlimit limit;
    getrlimit (RLIMIT_STACK, &limit);
    printf ("\nStack Limit = %llu and %llu max\n", limit.rlim_cur, limit.rlim_max);

    // get the length of distance pairs
    int trueLength = dictLength*dictLength;
    // array to hold precalculated distance pairs
    unsigned char allDistances[trueLength];
    // populate array with distance pairs
    printf("--------Initial Pass--------\n");
    for(int y = 0; y < dictLength; y++){
        // update user on progress
        if(y%50 == 0){
            printf("%i \n",y);
        }
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
        if(k%50 == 0){
            end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("%i%% time elapsed: %f seconds\n",(int)((k*100)/dictLength), time_spent);
            begin = clock();
        }
        // for each pair of words
        for(int i = 0; i < dictLength; i++){
            for(int j = 0; j < dictLength; j++){
                // if k's position as a midpoint is unknown, skip it
                if(allDistances[m(i,j)] == 0 || allDistances[m(i,j)] == 1 || allDistances[m(i,k)] == (unsigned char)255 || allDistances[m(k,j)] == (unsigned char)255){
                    continue;
                }
                /* otherwise, if the distance from i to j is greater than the distance 
                from i to k to j, then k provides a shortcut and i to j should be updated. */
                if(allDistances[m(i,j)] > allDistances[m(i,k)] + allDistances[m(k,j)]){
                    allDistances[m(i,j)] = allDistances[m(i,k)] + allDistances[m(k,j)];
                }
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
        if(y%50 == 0){
            printf("%i\n",y);
        }
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
    return 0;
}

// find all neighbours of a word
int main(int argc, char *argv[])
{
    // check num of arguments
    if(argc != 1){
        printf("Invalid number of arguments.\n");
        return 0;        
    }

    struct rlimit limit;
    getrlimit (RLIMIT_STACK, &limit);
    printf ("\nStack Limit = %llu and %llu max\n", limit.rlim_cur, limit.rlim_max);

    const rlim_t kStackSize = 50000000;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }

    getrlimit (RLIMIT_STACK, &limit);
    printf ("\nStack Limit = %llu and %llu max\n", limit.rlim_cur, limit.rlim_max);

    run();
    return 0;
}
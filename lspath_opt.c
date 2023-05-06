#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "global.c"
#include "hashset.h"
#include "quicksort.h"

// the length of the word
const int wordLength = 5;
// the length of the word with a newline
const int wordLengthSpace = wordLength+1;

// the length of the dictionary
const unsigned int dictLength = 5757;

// maps two dictionary indicies to one index in the precalculated distances array
int m(int x, int y){
    if(x > y){
        int temp = x;
        x = y;
        y = temp;
    }
    return (y*dictLength) + x;
}

// finds the longest shortest word ladder
int main(int argc, char *argv[])
{
    // an array containing all words from the text file
    struct hashset* wordSet = initialize_set(12000);
    char allWords[6000][wordLengthSpace];
    fast_cell allCells[6000];
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
        insert(line, wordSet);
    }
    // close the dictionary file
    fclose(file);
    
    char currentWord[wordLengthSpace];
    char newWord[wordLengthSpace];
    int openCellIDs[6000];
    struct fast_cell currentCell;

    int aloofWordsLength = 0;
    int numNeighbours = 0;
    int maxNeighbours = 0;
    int neighbourIndex = 0;

    // get the length of distance pairs
    int trueLength = dictLength*dictLength;

    // array to hold precalculated distance pairs
    unsigned char* allDistances = malloc(sizeof(unsigned char)*trueLength);
    check(allDistances);
    for(int i = 0; i < allWordsLength; i++){
        for(int j = 0; j < allWordsLength; j++){
            allDistances[m(i,j)] = 0;
        }
    }

    // initialise all word ids
    for(int k = 0; k < allWordsLength; k++){
        // initialise word
        get(allWords[k], wordSet)->id = k;
        allCells[k].id = k;
    }

    // connect all words to all word neighbours by ids
    for(int k = 0; k < allWordsLength; k++){
        // initialise word
        strcpy(currentWord, allWords[k]);
        numNeighbours = 0;
        neighbourIndex = 0;
        // loop over each letter
        for(int i = 0; i < wordLength; i++){
            // initialise word
            strcpy(newWord, currentWord);
            for(int j = 1; j < 26; j++){
                // mutate word
                newWord[i] = ((currentWord[i] - 97 + j) % 26) + 97;
                // if new word is invalid, skip
                if(!find(newWord, wordSet)){continue;}
                // otherwise, word has neighbours
                allCells[k].neighbours[neighbourIndex++] = get(newWord, wordSet)->id;
                numNeighbours++;
            }
        }
        allCells[k].numNeighbours = numNeighbours;
        maxNeighbours = numNeighbours > maxNeighbours ? numNeighbours : maxNeighbours;
    }
    printf("maxNeighbours: %i\n",maxNeighbours);

    /* here, do your time-consuming job */
    clock_t begin = clock();
    clock_t end;

    // for each word, dijkstra search to all neighbours
    int openCellsLength;
    bool finished;
    for(int start_i = 0; start_i < dictLength; start_i++){

        if(start_i % (dictLength/20) == 0){
            end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("%i%% complete - time elapsed: %.1f seconds\n",(int)((start_i*100)/dictLength), time_spent);
            begin = clock();
        }
    
        // initialise word
        for(int k = 0; k < allWordsLength; k++){
            allCells[k].closed = false;
            allCells[k].opened = false;
            allCells[k].fCost  = 0;
        }

        openCellsLength = 1;
        finished = false;

        int current_i = start_i;
        // perform dijkstra search
        while(!finished){
            // take lowest fCost from open cells
            allCells[current_i].closed = true;
            // loop over visisted cells and update distances
            allDistances[m(start_i, current_i)] = allCells[current_i].fCost;
            // remove currentCell from open cells
            openCellsLength--;

            // loop over each neighbour
            for(int i = 0; i < allCells[current_i].numNeighbours; i++){
                // evaluate new word and add to openCells
                openCellIDs[openCellsLength] = allCells[current_i].neighbours[i];
                // skip cell if closed
                if(allCells[openCellIDs[openCellsLength]].closed){
                    continue;
                }
                // if cell is open:
                if(allCells[openCellIDs[openCellsLength]].opened){
                    // update the cost and parent word if a lower cost is found
                    if(allCells[current_i].fCost + 1 < allCells[openCellIDs[openCellsLength]].fCost){
                        allCells[openCellIDs[openCellsLength]].fCost = allCells[current_i].fCost + 1;
                    }
                // if the cell is neither open nor closed: it is undiscovered
                }else{
                    // set cell as opened and update costs and parent
                    allCells[openCellIDs[openCellsLength]].opened = true;
                    allCells[openCellIDs[openCellsLength]].fCost = allCells[current_i].fCost + 1;
                    openCellsLength++;
                }
            }
            // if no cells left, end the search
            if(openCellsLength == 0){
                finished = true;
                continue;
            }
            // printf("before");
            // for(int a = 0; a < openCellsLength; a++){
            //     printf("%i: %i->%i\n",a,openCellIDs[a],allCells[openCellIDs[a]].fCost);
            // }
            // otherwise, sort the stach by fcost
            quickSortCellIDs(allCells, openCellIDs, 0, openCellsLength-1);
            // printf("after");
            // for(int a = 0; a < openCellsLength; a++){
            //     printf("%i: %i->%i\n",a,openCellIDs[a],allCells[openCellIDs[a]].fCost);
            // }
            // pick the cell from the top of the stack
            current_i = allCells[openCellIDs[openCellsLength-1]].id;
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
            //printf("%s -> %s dist: %i\n",allWords[x],allWords[y],allDistances[m(x,y)]);
        }
    }
    // display the words furthest apart
    printf("dist:%i %s %s\n",greatestDist,allWords[greatestX],allWords[greatestY]);
    // free the memory
    free(allDistances);
    tidy(wordSet);
    return 0;
}
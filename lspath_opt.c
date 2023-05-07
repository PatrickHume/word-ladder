#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "global.c"
#include "hashset.h"

// the length of the word
const int wordLength = 5;
// the length of the word with a newline
const int wordLengthSpace = wordLength+1;
// the length of the dictionary
const unsigned int dictLength = 5757;
// all cells used for dijkstra's algorithm
fast_cell allCells[dictLength];
int openCellIDs[dictLength];

// find furthest cell from cell at current_i using dijkstra's algorithm
int get_furthest_from(int current_i){
    int openCellsLength;
    bool finished;
    // initialise cells
    for(int k = 0; k < dictLength; k++){
        allCells[k].closed      = false;
        allCells[k].opened      = false;
        allCells[k].fCost       = 0;
    }
    // find word with most unknown reachables
    openCellsLength = 1;
    // run until finished
    finished = false;
    // perform dijkstra search
    while(!finished){
        // close and complete cell
        allCells[current_i].closed = true;
        allCells[current_i].complete = true;
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
            bool parentSet = false;
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
                // update distance matrix
                openCellsLength++;
            }
        }
        // if no cells left, checking furthest word on island
        if(openCellsLength == 0){
            finished = true;
            continue;
        }
        // otherwise pick next word with lowest fCost
        int minFCost = 5000;
        int lowestIndex = 0;
        current_i = allCells[openCellIDs[0]].id;
        for(int i = 0; i < openCellsLength; i++){
            if(allCells[openCellIDs[i]].fCost < minFCost){
                minFCost = allCells[openCellIDs[i]].fCost;
                lowestIndex = i;
            }
        }
        current_i = allCells[openCellIDs[lowestIndex]].id;
        openCellIDs[lowestIndex] = openCellIDs[openCellsLength - 1];
    }
    // return last cell closed
    return current_i;
}

// finds the longest shortest word ladder
int main(int argc, char *argv[])
{
    // an array containing all words from the text file
    struct hashset* wordSet = initialize_set(12000);
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
        insert(line, wordSet);
    }
    // close the dictionary file
    fclose(file);

    // variables for identifying neighbours
    char currentWord[wordLengthSpace];
    char newWord[wordLengthSpace];
    int neighbourIndex = 0;

    // initialise all word ids
    for(int k = 0; k < allWordsLength; k++){
        // initialise hashset and cells
        get(allWords[k], wordSet)->id = k;
        allCells[k].id = k;
    }

    // connect all words to all word neighbours by ids
    for(int k = 0; k < allWordsLength; k++){
        // initialise word
        strcpy(currentWord, allWords[k]);
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
                int id = get(newWord, wordSet)->id;
                // append neighbour id to array
                allCells[k].neighbours[neighbourIndex++] = id;
            }
        }
        // record num neighbours
        allCells[k].numNeighbours = neighbourIndex;
    }

    // initialise cells
    for(int k = 0; k < allWordsLength; k++){
        allCells[k].complete    = false;
    }

    // record max dist values
    int max_dist = 0;
    int max_dist_start_i;
    int max_dist_end_i;

    // search for longest shortest path
    while(true){
        int init_i;
        // find an untraversed cell
        bool found = false;
        for(int i = 0; i < dictLength; i++){
            if(!allCells[i].complete && allCells[i].numNeighbours != 0){
                init_i = i;
                found = true;
            }
        }
        // exit if all cells traversed or cell is aloof word
        if(!found){
            break;
        }
        // find cell furthest from some point
        int start_i = get_furthest_from(init_i);
        // find cell and to opposite end
        int end_i = get_furthest_from(start_i);
        // check if dist is longest so far
        if(allCells[end_i].fCost > max_dist){
            max_dist_start_i    = start_i;
            max_dist_end_i      = end_i;
            max_dist = allCells[end_i].fCost;
        }
    }

    // display the max dist
    printf("Longest word ladder: %s -> %s (%i steps)\n",allWords[max_dist_start_i], allWords[max_dist_end_i], max_dist);

    // free the memory
    tidy(wordSet);
    return 0;
}
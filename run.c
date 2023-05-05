#include "hashset.h"
#include "quicksort.h"
#include <stdio.h>
#include <string.h>

const int wordLength = 5;

// the heuristic, the minimum distance between two words
int h(char* a, char* b){
    int score = 0;
    for(int i = 0; i < wordLength; i++){
        score += (int)(a[i] != b[i]);
    }
    return score;
}

// find the distance between two words
int main(int argc, char *argv[])
{
    // check num of arguments
    if(argc != 3){
        printf("Invalid number of arguments.\n");
        return 0;        
    }
    // check argument length
    if(strlen(argv[1]) != wordLength || strlen(argv[2]) != wordLength){
        printf("Arguments must be of length %i.\n", wordLength);
        return 0;
    }
    // a hashset of all words in the dictionary
    struct hashset* wordSet = initialize_set(12000);
    // open dictionary file and write all words to the hashset
    FILE* file = fopen("allWords.txt", "r");
    char line[7];
    int counter = 0;
    const int maxWords = 5757;
    while (fgets(line, sizeof(line), file)) {
        if(counter>=maxWords){
            break;
        }
        /* note that fgets doesn't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        line[strcspn(line, "\n")] = 0;
        insert(line, wordSet);
        counter++;
    }
    // close the dictionary file
    fclose(file);

    // load current, target, and new words.
    char currentWord[6];
    char targetWord[6];
    char newWord[6];
    strcpy(targetWord, argv[1]);
    strcpy(currentWord, argv[2]);
    insert(currentWord, wordSet);
    insert(targetWord, wordSet);

    // create cells to perform A* search
    // openCells is a stack of cells to evaluate
    struct cell* openCells[6000];
    // currentCell is the cell being evaluated
    struct cell* currentCell;
    // steps keeps track of the number of iterations
    int iterations = 0;
    // found and failed record the status of the search
    bool found = false;
    bool failed = false;

    // perform A* search
    currentCell = get(currentWord,wordSet);
    int openCellsLength = 1;
    while(!found){
        iterations++;
        // take lowest fCost from open cells
        currentCell->closed = true;
        // remove currentCell from open cells
        openCellsLength--;
        // if found target then break out
        if(!strcmp(currentCell->element,targetWord)){
            found = true;
            continue;
        }
        // modify each character and check if new word is valid
        for(int i = 0; i < wordLength; i++){
            strcpy(newWord,currentCell->element);
            for(int j = 1; j < 26; j++){
                newWord[i] = (((currentCell->element)[i] - 97 + j) % 26) + 97;
                // if new word is valid, add to open cells
                if(find(newWord,wordSet)){
                    // evaluate new word and add to openCells
                    openCells[openCellsLength] = get(newWord,wordSet);
                    // skip cell if closed
                    if(openCells[openCellsLength]->closed){
                        continue;
                    }
                    // if cell is open:
                    if(openCells[openCellsLength]->opened){
                        // update the cost and parent word if a lower cost is found
                        if(currentCell->gCost + 1 < openCells[openCellsLength]->gCost){
                            openCells[openCellsLength]->gCost = currentCell->gCost + 1;
                            openCells[openCellsLength]->fCost = openCells[openCellsLength]->gCost + openCells[openCellsLength]->hCost;
                            openCells[openCellsLength]->parent = currentCell;
                        }
                    // if the cell is neither open nor closed: it is undiscovered
                    }else{
                        // set cell as opened and update costs and parent
                        openCells[openCellsLength]->opened = true;
                        openCells[openCellsLength]->gCost = currentCell->gCost + 1;
                        openCells[openCellsLength]->hCost = h(newWord,targetWord);
                        openCells[openCellsLength]->fCost = openCells[openCellsLength]->gCost + openCells[openCellsLength]->hCost;
                        openCells[openCellsLength]->parent = currentCell;
                        openCellsLength++;
                    }
                }
            }
        }
        // if no cells left, end the search
        if(openCellsLength == 0){
            found = true;
            failed = true;
            continue;
        }
        // otherwise, sort the stach by fcost
        quickSortIterative(openCells,0,openCellsLength-1);
        // pick the cell from the top of the stack
        currentCell = openCells[openCellsLength-1];
    }

    // display the number of iterations during the search
    printf("----------------------\n");
    printf("Search iterations: %i\n", iterations);
    printf("----------------------\n");
    
    // check if the search failed
    if(failed){
        printf("Search failed.\n");
    // otherwise find the path
    }else{
        // count the number of steps taken
        int steps = 0;
        // traverse up the chain of parents from start word until reached end word
        do{
            // display the child
            printf("%i: %s\n",steps, currentCell->element);
            // take the parent
            currentCell = currentCell->parent;
            // count the step
            steps++;
        }while(strcmp(currentCell->element, currentWord));
        // output the end word
        printf("%i: %s\n",steps, currentCell->element);
    }
    // free the memory
    tidy(wordSet);
    return 0;
}
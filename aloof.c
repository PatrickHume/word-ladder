#include "hashset.h"    // a hashset providing .insert and .find
#include <stdio.h>      // for printing
#include <string.h>     // for strcpy
// the length of the word
const int wordLength = 5;
// the length of the word with a newline
const int wordLengthSpace = wordLength+1;

// find all neighbours of a word
int main(int argc, char *argv[])
{
    // check num of arguments
    if(argc != 1){
        printf("Invalid number of arguments.\n");
        return 0;        
    }
    // a hashset of all words in the dictionary
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
    
    char currentWord[wordLengthSpace];
    char newWord[wordLengthSpace];
    char aloofWords[6000][wordLengthSpace];
    int aloofWordsLength = 0;
    bool hasNeighbours = false;
    // find every aloof word
    for(int k = 0; k < allWordsLength; k++){
        // initialise word
        strcpy(currentWord, allWords[k]);
        hasNeighbours = false;
        // loop over each letter
        for(int i = 0; i < wordLength; i++){
            // initialise word
            strcpy(newWord, currentWord);
            for(int j = 1; j < 26; j++){
                // mutate word
                newWord[i] = ((currentWord[i] - 97 + j) % 26) + 97;
                // if new word is invalid, skip
                if(!find(newWord, wordSet)){continue;}
                // otherwise, word has neighbours, so we can skip word
                hasNeighbours = true;
                break;
            }
            // if has neighbours, skip
            if(hasNeighbours){break;}
        }
        // if has neighbours, skip
        if(hasNeighbours){continue;}
        // otherwise record word
        strcpy(aloofWords[aloofWordsLength++], currentWord);
    }

    // display aloof words
    printf("%i aloof words:\n", aloofWordsLength);
    for(int i = 0; i < aloofWordsLength; i++){
        printf("%s\n", aloofWords[i]);
    }

    // free the memory
    tidy(wordSet);
    return 0;
}
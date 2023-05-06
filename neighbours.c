#include "neighbours.h"    // a hashset providing .insert and .find
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
    if(argc != 2){
        printf("Invalid number of arguments.\n");
        return 0;        
    }
    // check argument length
    if(strlen(argv[1]) != wordLength){
        printf("Argument must be of length %i.\n", wordLength);
        return 0;
    }
    // a hashset of all words in the dictionary
    struct hashset* wordSet = initialize_set(12000);
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
        insert(line, wordSet);
    }
    // close the dictionary file
    fclose(file);

    // load current, target, and new words.
    char currentWord[wordLengthSpace];
    char newWord[wordLengthSpace];
    strcpy(currentWord, argv[1]);
    insert(currentWord, wordSet);
    // neighbouring cells array
    char neighbouringWords[1000][wordLengthSpace];
    int neighbouringWordsLength = 0;

    // find all neighbours
    // loop over each letter
    for(int i = 0; i < wordLength; i++){
        // initialise word
        strcpy(newWord, currentWord);
        for(int j = 1; j < 26; j++){
            // mutate word
            newWord[i] = ((currentWord[i] - 97 + j) % 26) + 97;
            // if new word is invalid, skip
            if(!find(newWord, wordSet)){continue;}
            // otherwise record word
            strcpy(neighbouringWords[neighbouringWordsLength++], newWord);
        }
    }

    // display neightbouring words
    printf("%i neighbours:\n", neighbouringWordsLength);
    for(int i = 0; i < neighbouringWordsLength; i++){
        printf("%s\n", neighbouringWords[i]);
    }

    // free the memory
    tidy(wordSet);
    return 0;
}
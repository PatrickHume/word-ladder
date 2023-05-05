#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define __USE_BSD
#include <string.h>
#include <math.h>

#include "global.h"
#include "hashset.h"

int mode = 2;

// Can be redefined if Value_Type changes
int compare(Value_Type a, Value_Type b){
  return strcmp(a,b);
}

// Helper functions for finding prime numbers 
bool isPrime (int n)
{
  for (int i = 2; i*i <= n; i++)
    if (n % i == 0)
      return false;
  return true;
}

int nextPrime(int n)
{
  for (; !isPrime(n); n++);
  return n;
}

// Your code
// Zero all hashset attributes
struct hashset* initialize_set (int size)  
{
  struct hashset* set;
  set = malloc(sizeof(struct hashset));
  set->size = nextPrime(size);
  set->initial_size = set->size;

  set->num_entries = 0;
  set->num_inserts = 0;
  set->num_collisions = 0;
  set->num_rehashes = 0;
  set->num_finds = 0;
  set->num_resizes = 0;

  // allocate cells to memory
  set->cells = malloc(size*sizeof(cell));
  for(int i = 0; i < size; i++){
    set->cells[i].gCost = 0;
    set->cells[i].fCost = 0;
    set->cells[i].hCost = 0;
    set->cells[i].parent = NULL;
    set->cells[i].opened = false;
    set->cells[i].closed = false;
    set->cells[i].state = empty;
    set->cells[i].chain = NULL;
  }
  return set;
}

// Generate hash value by summation of char values
int sum_hash(char* value, int size)
{
  int sum = 0;
  while(*value != 0){
    sum = sum + (int)(*value);
    value++;
  }
  return abs(sum % size);
}

// Generate hash value
int djb2_hash(char* value, int size){
    // prime 5381 is known to produce good results
    unsigned long hash = 5381;
    int c;
    while ((c = *value++))
        // value is shifted to account for char values all being relatively low
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % size;
}

// Linear probing rehash - add 1 to hashValue
int linear_probing(int hashValue, int size, int* attempt_number){
  (*attempt_number)++;
  return abs((hashValue + *attempt_number) % size);
}

// Quadratic probing rehash - Hashvalue + 0.5n + 0.5n^2
int quadratic_probing(int hashValue, int size, int* attempt_number){
  (*attempt_number)++;
  return abs((hashValue + (int)(0.5f*(*attempt_number)) + (int)(0.5f*pow((*attempt_number),2))) % size);
}

// Integer hashing function,
// produces no two same outputs for different inputs
int second_hash(int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    if(x == 0){
      x = 0x45d9f3b;
    }
    return abs(x);
}

// rehash hashValue with second hashing function
int double_hashing(int hashValue, int size, int* attempt_number){
  (*attempt_number)++;
  return abs((hashValue + ((*attempt_number) * second_hash(hashValue))) % size);
}

int resize_regular(cell* chainLink,struct hashset* set){
  return 0;
}

int resize_chaining(cell* chainLink,struct hashset* set){
  struct cell* chainCell = chainLink->chain;
  if(chainCell){
    while(chainCell->chain){
      free(chainCell->chain->element);
      struct cell* tempChainCell = chainCell->chain;
      free(chainCell);
      chainCell = tempChainCell;
    }
  }
  return 0;
}

// Return function pointer for rehash function
int (*set_rehash(int mode))(int,int,int*){
  switch(mode){
    case HASH_1_LINEAR_PROBING:
    case HASH_2_LINEAR_PROBING:
      return &linear_probing;
    case HASH_1_QUADRATIC_PROBING:
    case HASH_2_QUADRATIC_PROBING:
      return &quadratic_probing;
    case HASH_1_DOUBLE_HASHING:
    case HASH_2_DOUBLE_HASHING:
      return &double_hashing;
    case HASH_1_SEPARATE_CHAINING:
    case HASH_2_SEPARATE_CHAINING:
    default:
      return &linear_probing;
  }
}

// Return function pointer for hash function
int (*set_hash(int mode))(char*,int){
  switch(mode){
    case HASH_1_LINEAR_PROBING:
    case HASH_1_QUADRATIC_PROBING:
    case HASH_1_DOUBLE_HASHING:
    case HASH_1_SEPARATE_CHAINING:
      return &djb2_hash;
    case HASH_2_LINEAR_PROBING:
    case HASH_2_QUADRATIC_PROBING:
    case HASH_2_DOUBLE_HASHING:
    case HASH_2_SEPARATE_CHAINING:
      return &sum_hash;
    default:
      return &djb2_hash;
  }
}

// Return function pointer for hash function
int (*set_resize(int mode))(cell*,struct hashset*){
  switch(mode){
    case HASH_1_SEPARATE_CHAINING:
    case HASH_2_SEPARATE_CHAINING:
      return &resize_chaining;
    default:
      return &resize_regular;
  }
}

// Free set cells from memory and free set
void tidy(struct hashset* set)
{
  int (*resize)(cell*,struct hashset*) = set_resize(mode);

  int size = set->size;
  for(int i = 0; i < size; i++){
    if(set->cells[i].state != empty){
      // --- If seperate chaining is enabled ---
      // Go down the chain freeing each cell until null cell reached
      resize(&set->cells[i],set);
      free(set->cells[i].element);
    }
  }
  free(set->cells);
  free(set);
}

// With seperate chaining being different from the other collision avoidance strategies, 
// the rehashing in both insert and find functions are split into two parts for seperate chaining and otherwise.

// Insert value into hashset
struct hashset* insert (Value_Type value, struct hashset* set)
{
  // Select hash and rehash function relative to mode
  int (*hash)(char*,int)      = set_hash(mode);
  int (*rehash)(int,int,int*) = set_rehash(mode);
  // Resize differently if mode involves seperate chaining
  int (*resize)(cell*,struct hashset*) = set_resize(mode);
  
  // If hashset is over half full, resize by next prime of doubled size
  if(set->num_entries * 2 >= set->size){
    set->num_resizes++;
    cell* oldArray = set->cells;
    int oldSize = set->size;
    set->size *= 2;
    set->size = nextPrime(set->size);
    set->cells = malloc(set->size * sizeof(cell));
    // if malloc successful
    if(set->cells){ // insert old elements into new hashset
        // initialise new cells
        for(int i = 0; i < set->size; i++){
          set->cells[i].state = empty;
          set->cells[i].chain = NULL;
        }
        set->num_entries = 0;
        // insert values from old cells into new resized cells
        for(int i = 0; i < oldSize; i++){
          if(oldArray[i].state != empty){
            insert(oldArray[i].element,set);
            // resize differently if using seperate chaining
            resize(&oldArray[i],set);
            free(oldArray[i].element);
          }
        }
        // free now unused old cells
        free(oldArray);
    }else{ // exit if malloc failed
        printf("resize malloc failed.\n");
        exit(0);
    }
  }

  // Generate hash value and scale to size of hashset
  int hashValue = (*hash)(value,set->size);

  // Check cell at location of hashValue 
  // If this cell is empty:
  if(set->cells[hashValue].state != in_use){
    // Assign value to location in hashtable
    set->cells[hashValue].element = malloc((strlen(value)+1)*sizeof(Value_Type));
    strcpy(set->cells[hashValue].element,value);
    set->cells[hashValue].state = in_use;
  }// Otherwise if value is different
  else if(strcmp(set->cells[hashValue].element,value) != 0)
  {
    // --- If seperate chaining is enabled ---
    switch(mode){
      case HASH_1_SEPARATE_CHAINING:
      case HASH_2_SEPARATE_CHAINING:
        {
        // Traverse down the chain of cells looking for the value
        struct cell* chainCell = &(set->cells[hashValue]);
        while(chainCell->chain){
          if(strcmp((chainCell->chain)->element,value) == 0){
            return set;
          }
          chainCell = chainCell->chain;
        }
        // If end of chain reached, add a new cell to the end of the chain
        chainCell->chain = malloc(sizeof(struct cell));
        chainCell->chain->chain = NULL;
        chainCell->chain->element = malloc((strlen(value)+1)*sizeof(Value_Type));
        strcpy(chainCell->chain->element,value);
        chainCell->chain->state = in_use;
        set->num_entries--;
        break;
        }

    // --- Otherwise rehash as normal ---
      default:
        {
        // Rehash until reached a cell not in use
        int attempt_number = 0;
        int rehashValue;
        do{
          set->num_collisions++;
          rehashValue = (*rehash)(hashValue,set->size,&attempt_number);
        }while(set->cells[rehashValue].state == in_use);
        // Once empty cell is found
        // Assign value to location in hashtable
        set->cells[rehashValue].element = malloc((strlen(value)+1)*sizeof(Value_Type));
        strcpy(set->cells[rehashValue].element,value);
        set->cells[rehashValue].state = in_use;
        break;
        }
    }
  }else{// Else this value is a duplicate already in the hashset
    return set; // So just return before "number of entries" counter is incremented
  }

  // increase the counter of number of entries inserted to the hashset
  set->num_entries++;
  set->num_inserts++;
  return set;
}

// Find value in hashset
bool find (Value_Type value, struct hashset* set)
{
  set->num_finds++;
  // Select hash and rehash function relative to mode
  int (*rehash)(int,int,int*) = set_rehash(mode);
  int (*hash)(char*,int)      = set_hash(mode);

  // Generate hash value and scale to size of hashset
  int hashValue = (*hash)(value,set->size);

  // Check cell at location of hashValue 
  // If this cell is empty:
  if(set->cells[hashValue].state != in_use)
  { // Then no value is here in hashset
    return false;
  }
  else
  { // Some value is here in hashset

    // --- If seperate chaining is enabled ---
    switch(mode){
      case HASH_1_SEPARATE_CHAINING:
      case HASH_2_SEPARATE_CHAINING:
        {
        // Traverse down the chain of cells until value reached
        struct cell* chainCell = &(set->cells[hashValue]);
        while(strcmp(chainCell->element,value) != 0){
          chainCell = chainCell->chain;
          if(!chainCell){
            // Fail if end of chain reached without finding value
            return false;
          }
        }
        break;
        }

    // --- Otherwise rehash as normal ---
      default:
        {
        int attempt_number = 0;
        int rehashValue = hashValue;
        // Rehash until cell found matching value
        while(strcmp(set->cells[rehashValue].element, value) != 0)
        {
          set->num_rehashes++;
          rehashValue = (*rehash)(hashValue,set->size, &attempt_number);
          // If rehashed into empty cell
          if(set->cells[rehashValue].state == empty){
            return false; // Then value doesn't exist in hashset
          }
        }
        break;
        }
    }
    // Value has been found in hashset
    return true;
  }
}

// Get value in hashset
struct cell* get (Value_Type value, struct hashset* set)
{
  set->num_finds++;
  // Select hash and rehash function relative to mode
  int (*rehash)(int,int,int*) = set_rehash(mode);
  int (*hash)(char*,int)      = set_hash(mode);

  // Generate hash value and scale to size of hashset
  int hashValue = (*hash)(value,set->size);

  int attempt_number = 0;
  int rehashValue = hashValue;
  // Rehash until cell found matching value
  while(strcmp(set->cells[rehashValue].element, value) != 0)
  {
    set->num_rehashes++;
    rehashValue = (*rehash)(hashValue,set->size, &attempt_number);
    // If rehashed into empty cell
    if(set->cells[rehashValue].state == empty){
      return NULL; // Then value doesn't exist in hashset
    }
  }
  // Value has been found in hashset
  return &(set->cells[rehashValue]);
}

// Print structure of hashtable
void print_set (struct hashset* set)
{
  printf("Start Stats\n");
  // --- If seperate chaining is enabled ---
  switch(mode){
    case HASH_1_SEPARATE_CHAINING:
    case HASH_2_SEPARATE_CHAINING:
      {
        for(int i = 0; i < set->size; i++)
        {
          switch (set->cells[i].state)
          {
            case empty:
              {printf("empty   - %i: \n",i);}
            break;
            case in_use:
              {
              printf("in use  - %i: %s",i,set->cells[i].element);
              struct cell* chainCell = &(set->cells[i]);
              while(chainCell->chain){
                printf(" -> %s",chainCell->chain->element);
                chainCell = chainCell->chain;
              }
              printf("\n");
            break;
            }
            case deleted:
              {
              printf("deleted - %i: %s\n",i,set->cells[i].element);
              struct cell* chainCell = set->cells[i].chain;
              while(chainCell){
                printf(" -> %s",chainCell->element);
                chainCell = chainCell->chain;
              }
              printf("\n");
            break;
              }
          }
        }
        break;
      }

    // --- Otherwise print as normal ---
    default:
      {
        for(int i = 0; i < set->size; i++)
        {
          switch (set->cells[i].state)
          {
            case empty:
              printf("empty   - %i: \n",i);
            break;
            case in_use:
              printf("in use  - %i: %s\n",i,set->cells[i].element);
            break;
            case deleted:
              printf("deleted - %i: %s\n",i,set->cells[i].element);
            break;
          }
        }
        break;
      }
  }
  printf("End Stats\n");
}

// Print hashtable stats
void print_stats (struct hashset* set)
{
  float average_collisions = (float)set->num_collisions/(float)set->num_inserts;
  float average_rehashes = (float)set->num_rehashes/(float)set->num_finds;
  printf("Arr. original size: ---------- %i\n",  set->initial_size);
  printf("Arr. size: ------------------- %i\n|\n",  set->size);

  printf("No.  inserts: ---------------- %i\n",  set->num_inserts);
  printf("No.  collisions: ------------- %i\n",  set->num_collisions);
  printf("Avg. collisions per insert: -- %f\n|\n",  average_collisions);

  printf("No.  finds: ------------------ %i\n",  set->num_finds);
  printf("No.  rehashes: --------------- %i\n",  set->num_rehashes);
  printf("Avg. rehashes per find: ------ %f\n|\n",  average_rehashes);

  printf("No.  resizes: ---------------- %i\n|\n",  set->num_resizes);
}

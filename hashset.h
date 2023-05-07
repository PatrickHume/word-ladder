// Giles Reger, 2019
#ifndef HASHSET_H
#define HASHSET_H

#include <stdbool.h> 

enum HashingModes { HASH_1_LINEAR_PROBING=0, 
                    HASH_1_QUADRATIC_PROBING=1, 
                    HASH_1_DOUBLE_HASHING=2, 
                    HASH_1_SEPARATE_CHAINING=3,
                    HASH_2_LINEAR_PROBING=4, 
                    HASH_2_QUADRATIC_PROBING=5, 
                    HASH_2_DOUBLE_HASHING=6, 
                    HASH_2_SEPARATE_CHAINING=7};

typedef char* Value_Type;
// Should be redefined if changing Value_Type
int compare(Value_Type,Value_Type);

typedef struct fast_cell
{ // hash-table entry
  int fCost;
  bool opened;
  bool closed;
  bool complete;
  int neighbours[30];
  int numNeighbours;
  int id;
} fast_cell;

// This is a cell struct assuming Open Addressing
// You will need alternative data-structurs for separate chaining
typedef struct cell
{ // hash-table entry
  Value_Type element;
  struct cell* parent;
  int gCost;
  int hCost;
  int fCost;
  bool opened;
  bool closed;
  int neighbours[30];
  int numNeighbours;
  int id;

  enum {empty, in_use, deleted} state;
  struct cell* chain;
} cell;


struct  hashset
{
  cell *cells; 
  int size; // cell cells [table_size];
  int num_entries; // number of cells in_use
  int num_inserts;
  int initial_size;
  int num_finds;      // number of times find is called
  int num_collisions; // number of times rehash is called in insert
  int num_rehashes;   // number of times rehash is called in find
  int num_resizes;    // number of times set is resized
};

struct hashset* initialize_set (int size);     
void tidy (struct hashset*); 

struct hashset* insert (Value_Type, struct hashset*);

bool find (Value_Type, struct hashset*);
struct cell* get (Value_Type, struct hashset*);
void reset_cells(struct hashset* set);

// Helper functions
void print_set (struct hashset*);
void print_stats (struct hashset*);

#endif
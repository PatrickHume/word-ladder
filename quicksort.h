#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "hashset.h"

struct cell;

void swap_fast(int* a, int* b);
int partition_fast(fast_cell allCells[6000], int CellIDs[6000], int l, int h);

void swap(struct cell** a, struct cell** b);
 
/* This function is same in both iterative and recursive*/
int partition(struct cell* arr[], int l, int h);
 
/* A[] --> Array to be sorted,
   l  --> Starting index,
   h  --> Ending index */
void quickSortIterative(struct cell* arr[], int l, int h);
void quickSortCellIDs(fast_cell allCells[6000], int CellIDs[6000], int l, int h);
 
// A utility function to print contents of arr
void printArr(struct cell* arr[], int n);

#endif
#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "hashset.h"

struct cell;

void swap(struct cell** a, struct cell** b);
 
/* This function is same in both iterative and recursive*/
int partition(struct cell* arr[], int l, int h);
 
/* A[] --> Array to be sorted,
   l  --> Starting index,
   h  --> Ending index */
void quickSortIterative(struct cell* arr[], int l, int h);

// A utility function to print contents of arr
void printArr(struct cell* arr[], int n);

#endif
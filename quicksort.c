#include "quicksort.h"

void swap_fast(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
 
/* This function is same in both iterative and recursive*/
int partition_fast(fast_cell allCells[6000], int CellIDs[6000], int l, int h)
{
    int x = CellIDs[h];
    int i = (l - 1);
 
    for (int j = l; j <= h - 1; j++) {
        if (allCells[CellIDs[j]].fCost >= allCells[x].fCost) {
            i++;
            swap_fast(&(CellIDs[i]), &(CellIDs[j]));
        }
    }
    swap_fast(&(CellIDs[i+1]), &(CellIDs[h]));
    return (i + 1);
}
 
/* A[] --> Array to be sorted,
   l  --> Starting index,
   h  --> Ending index */
void quickSortCellIDs(fast_cell allCells[6000], int CellIDs[6000], int l, int h)
{
    // Create an auxiliary stack
    int stack[h - l + 1];
 
    // initialize top of stack
    int top = -1;
 
    // push initial values of l and h to stack
    stack[++top] = l;
    stack[++top] = h;
 
    // Keep popping from stack while is not empty
    while (top >= 0) {
        // Pop h and l
        h = stack[top--];
        l = stack[top--];
 
        // Set pivot element at its correct position
        // in sorted array
        int p = partition_fast(allCells, CellIDs, l, h);
 
        // If there are elements on left side of pivot,
        // then push left side to stack
        if (p - 1 > l) {
            stack[++top] = l;
            stack[++top] = p - 1;
        }
 
        // If there are elements on right side of pivot,
        // then push right side to stack
        if (p + 1 < h) {
            stack[++top] = p + 1;
            stack[++top] = h;
        }
    }
}

void swap(struct cell** a, struct cell** b)
{
    struct cell* t = *a;
    *a = *b;
    *b = t;
}
 
/* This function is same in both iterative and recursive*/
int partition(struct cell* arr[], int l, int h)
{
    struct cell* x = arr[h];
    int i = (l - 1);
 
    for (int j = l; j <= h - 1; j++) {
        if (arr[j]->fCost >= x->fCost) {
            i++;
            swap(&(arr[i]), &(arr[j]));
        }
    }
    swap(&(arr[i+1]), &(arr[h]));
    return (i + 1);
}

/* A[] --> Array to be sorted,
   l  --> Starting index,
   h  --> Ending index */
void quickSortIterative(struct cell* arr[], int l, int h)
{
    // Create an auxiliary stack
    int stack[h - l + 1];
 
    // initialize top of stack
    int top = -1;
 
    // push initial values of l and h to stack
    stack[++top] = l;
    stack[++top] = h;
 
    // Keep popping from stack while is not empty
    while (top >= 0) {
        // Pop h and l
        h = stack[top--];
        l = stack[top--];
 
        // Set pivot element at its correct position
        // in sorted array
        int p = partition(arr, l, h);
 
        // If there are elements on left side of pivot,
        // then push left side to stack
        if (p - 1 > l) {
            stack[++top] = l;
            stack[++top] = p - 1;
        }
 
        // If there are elements on right side of pivot,
        // then push right side to stack
        if (p + 1 < h) {
            stack[++top] = p + 1;
            stack[++top] = h;
        }
    }
}

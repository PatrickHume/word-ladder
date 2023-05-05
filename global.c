#include <stdio.h>
#include <stdlib.h>

#include "global.h"

void check (void *memory)
{ // check result from strdup, malloc etc.
  if (memory == NULL)
  {
    fprintf (stderr, "out of memory\n");
    exit (3);
  }
}


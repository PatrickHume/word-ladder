# Use clang if it is present
ifeq ($(shell which clang),)
	CC := gcc
else
	CC := clang
endif
CFLAGS=-O3

all: journey lspath neighbours aloof

journey lspath neighbours aloof: %: %.o hashset.o quicksort.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o journey lspath neighbours aloof
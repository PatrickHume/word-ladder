CC=clang
CFLAGS=-O3

all: journey lspath neighbours aloof

journey lspath neighbours aloof: %: %.o hashset.o quicksort.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o journey lspath neighbours aloof
CC = clang
CFLAGS = -Wall -Wextra -Wpedantic

all: journey lspath neighbours aloof

journey: run.o hashset.o quicksort.o
	$(CC) $(CFLAGS) $^ -o $@

run.o: run.c hashset.h quicksort.h
	$(CC) $(CFLAGS) -c $< -o $@

hashset.o: hashset.c hashset.h global.h
	$(CC) $(CFLAGS) -c $< -o $@

quicksort.o: quicksort.c quicksort.h hashset.h
	$(CC) $(CFLAGS) -c $< -o $@

lspath: longestshortest.o
	$(CC) $(CFLAGS) $^ -o $@

longestshortest.o: longestshortest.c global.h
	$(CC) $(CFLAGS) -c $< -o $@

neighbours: neighbours.o hashset.o
	$(CC) $(CFLAGS) $^ -o $@

neighbours.o: neighbours.c hashset.h global.h
	$(CC) $(CFLAGS) -c $< -o $@

aloof: aloof.o hashset.o
	$(CC) $(CFLAGS) $^ -o $@

aloof.o: aloof.c hashset.h global.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o journey lspath neighbours aloof
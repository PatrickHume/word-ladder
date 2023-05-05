all: journey lspath neighbours aloof

journey: run.o hashset.o quicksort.o
	clang run.o hashset.o quicksort.o -o journey

run.o: run.c hashset.h quicksort.h
	clang -c run.c -o run.o

hashset.o: hashset.c hashset.h global.h
	clang -c hashset.c -o hashset.o

quicksort.o: quicksort.c quicksort.h hashset.h
	clang -c quicksort.c -o quicksort.o

lspath: longestshortest.o
	clang -Ofast longestshortest.o -o lspath

longestshortest.o: longestshortest.c global.h
	clang -Ofast -c longestshortest.c -o longestshortest.o

neighbours: neighbours.o hashset.o
	clang -O3 neighbours.o hashset.o -o neighbours

neighbours.o: neighbours.c hashset.h global.h
	clang -O3 -c neighbours.c -o neighbours.o

aloof: aloof.o hashset.o
	clang -O3 aloof.o hashset.o -o aloof

aloof.o: aloof.c hashset.h global.h
	clang -O3 -c aloof.c -o aloof.o

clean:
	rm -rf *.o journey

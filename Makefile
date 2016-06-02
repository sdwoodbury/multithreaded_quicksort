CC=gcc
FLAGS=-pthread -g -lm
WKPOOL=quicksort

$(WKPOOL): quicksort.c queue.h
	$(CC) quicksort.c -o $(WKPOOL) $(FLAGS)

clean:
	rm -f $(WKPOOL)

run:
	./$(WKPOOL) $1 $2 $3

x:
	make clean && make quicksort && ./quicksort 2 unsorted.txt sorted.txt

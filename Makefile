CC=gcc
FLAGS=-pthread -g -lm
WKPOOL=quicksort

$(WKPOOL): quicksort.c queue.h
	$(CC) quicksort.c -o $(WKPOOL) $(FLAGS)

clean:
	rm -f $(WKPOOL)


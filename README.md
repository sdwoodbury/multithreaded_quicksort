# workpool_quicksort

message format:   start			end			meaning
									-1				-1			waiting
									-1				0				terminate
									0					-1			working

master thread communicated to slave threads through mailboxes, protected by lockboxes
all threads busywait
when threads are done, they can all get a lock on the queue to give it work. 

usage: ./quicksort <number of threads (must be greater than 1)> <unsorted file> <output file>


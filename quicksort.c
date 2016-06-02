#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

struct qnode *mailbox;
pthread_mutex_t *lock;

pthread_mutex_t qlock;

int listSize;
int numThreads;
int *list;

int* getList(char *fileName){
	FILE *file = fopen(fileName, "r");
	int size = 0;
	char temp;
	while(!feof(file)){
		temp = fgetc(file);
		if(temp == '\n') {size++; listSize++;}
	}
	int *list = (int*)malloc(sizeof(int) * size);
	fseek(file, 0, SEEK_SET);
	
	int counter = 0;
	while(size-- > 0){
		fscanf(file, "%d", &list[counter++]);
	}
	fclose(file);
	return list;
}

void writeList(char *fileName, int *list){
	int counter = 0;
	FILE *file = fopen(fileName, "w");
	while(counter < listSize){
		fprintf(file, "%d\n", list[counter]);
		counter++;
	}
}

int partition(int* A, int p, int r){
	//int z = (rand() % (r - p + 1)) + p;
	//printf("z is %d\n", z);
	int x = A[r];
	int i = p - 1;
	int j = -1;
	int temp = 0;
	for(j = p; j < r; j++){
		if(A[j] <= x){
			i += 1;
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;
		}
	}

	i += 1;
	temp = A[i];
	A[i] = A[r];
	A[r] = temp;

	return i;
}


void *master(void *arg){
	
	int done = 0; //used for termination condition
	int c = 0;

	//put first piece of work in queue
	pthread_mutex_lock(&qlock);
	insert(0, listSize - 1);
	pthread_mutex_unlock(&qlock);

	while(done < numThreads - 1){
		//check mailboxes and do work
		done = 0;
		for(c = 1; c < numThreads; c++){
			pthread_mutex_lock(&lock[c]);
			if(mailbox[c].start == 0 && mailbox[c].end == -1){ //working
				//do nothing
			} else if(mailbox[c].start == -1 && mailbox[c].end == -1){ //waiting
				pthread_mutex_lock(&qlock);
				mailbox[c] = popq();
				pthread_mutex_unlock(&qlock);
			}
			if(mailbox[c].start == -1 && mailbox[c].end == -1){ //no work available
				done++;
			} //otherwise mailbox should say done
			pthread_mutex_unlock(&lock[c]);
		} //end for
	} //end while
//printf("done\n");
	for(c = 1; c < numThreads; c++){
		pthread_mutex_lock(&lock[c]);
		mailbox[c].start = -1;
		mailbox[c].end = 0;
		pthread_mutex_unlock(&lock[c]);
	}
	
	return NULL;
}

void *slave(void *arg){
	int id = *(int*)arg;
	int q = -1;
	
	struct qnode work;

	pthread_mutex_lock(&lock[id]);
	work = mailbox[id];
	pthread_mutex_unlock(&lock[id]);

	do{ //handle work

		if(work.start == -1 && work.end == -1){
			//do nothing
		} else if (work.start == -1 && work.end == 0){
			//terminate
			break;
		} else if(work.start >= 0 && work.end < listSize){
			//do work
			//change flag to 'working'
			pthread_mutex_lock(&lock[id]);
			mailbox[id].start = 0;
			mailbox[id].end = -1;
			pthread_mutex_unlock(&lock[id]);

			while(work.start < work.end){
				//printf("id : %d work: %d %d\n", (int)id, work.start, work.end);
				q = partition(list, work.start, work.end);
				if(q + 1 < work.end){
					pthread_mutex_lock(&qlock);
					insert(q + 1, work.end);
					pthread_mutex_unlock(&qlock);
				}
				work.end = q - 1;
			} //end while
			
			pthread_mutex_lock(&lock[id]);
			mailbox[id].start = -1;
			mailbox[id].end = -1;
			pthread_mutex_unlock(&lock[id]);
			
		} else {
			printf("error in thread %d\n", id);
			break;
		}

		//get more work
		pthread_mutex_lock(&lock[id]);
		work = mailbox[id];
		pthread_mutex_unlock(&lock[id]);
	} while(work.start != -1 || work.end != 0);

	return NULL;
	
}//end slave

void *bootstrap(void *arg){
	int id = *(int*)arg;
	if(id == 0){
		master(&id);
	} else {
		slave(&id);
	}
}

/*
command line args: threads, infile, outfile
*/
int main(int argc, char** argv){

	listSize = 0;
	srand(time(NULL));

	initQueue();

	list = getList(argv[2]);
	numThreads = atoi(argv[1]);
	pthread_t pool[numThreads];
	mailbox = malloc(sizeof(struct qnode) * numThreads);
	lock = malloc(sizeof(pthread_mutex_t) * numThreads);

	pthread_mutex_init(&qlock, NULL);

	int t = 0;
	int *args = malloc(sizeof(int) * numThreads);
	for(; t < numThreads; t++){
		args[t] = t;
		pthread_mutex_init(&lock[t], NULL);
		mailbox[t].start = -1;
		mailbox[t].end = -1;
		pthread_create(&pool[t], NULL, &bootstrap, &args[t]);
	}

	for(t = 0; t < numThreads; t++){
		pthread_join(pool[t], NULL);
		pthread_mutex_destroy(&lock[t]);
	}

	pthread_mutex_destroy(&qlock);

	writeList(argv[3], list);
	free(list);
	free(args);
	free(lock);
	free(mailbox);

	destroyQueue();

	return 0;
}

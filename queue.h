
#ifndef queue_h
#define queue_h

#include <math.h>
#include <stdlib.h>
#include <assert.h>

#define ERR -1
#define SUCCESS 1
#define MIN_SIZE 10

struct qnode{
	int start;
	int end;
};

struct qnode ** queue;
int next;
int size;

void initQueue(){
	size = MIN_SIZE;
	queue = malloc(sizeof(struct qnode *) * MIN_SIZE);
	next = 0;
}

void destroyQueue(){
	int counter = 0;
	for(; counter < next; counter++){
		free(queue[counter]);
	}
	free(queue);
}

void heapify(int n){

	if(n <= 0){
		return;
	}

	int parent = (int)log2((long)n);
	if(queue[parent]->end - queue[parent]->start < queue[n]->end - queue[n]->start){
		struct qnode *temp = queue[parent];
		queue[parent] = queue[n];
		queue[n] = temp;

		if(parent > 0){
			heapify(parent);
		}
	}
}

int insert(int start, int end){
	if(next > size){
		//realloc
		struct qnode ** temp = realloc(queue, size * 2 * sizeof(struct qnode *));
		if(temp != NULL){
			size *= 2;
			queue = temp;
		}
		else{
			return ERR;
		}
	}
	queue[next] = malloc(sizeof(struct qnode));
	queue[next]->start = start;
	queue[next]->end = end;

	next++;

	heapify(next - 1);
	return SUCCESS;
}

void percDown(int n){

	int left = 2 * n + 1;
	int right = 2 * n + 2;

	if(left >= next && right >= next){
		return;
	}

	int nlen = queue[n]->end - queue[n]->start,
	 llen = queue[left]->end - queue[left]->start;

	struct qnode *temp = queue[n];

	if(right >= next){ //have to make sure there's a right node
		if(nlen >= llen){
			return;
		}
		queue[n] = queue[left];
		queue[left] = temp;
		percDown(left);
		return;
	}

	int rlen = queue[right]->end - queue[right]->start;

	if(nlen >= llen && nlen >= rlen ){
		return;
	}

	if(llen > rlen){
		queue[n] = queue[left];
		queue[left] = temp;
		percDown(left);
	} else {
		queue[n] = queue[right];
		queue[right] = temp;
		percDown(right);
	}
}

struct qnode popq(){

	struct qnode ret;
	
	if(next == 0){
		ret.start = -1;
		ret.end = -1;
		return ret;
	}

	ret.end = queue[0]->end;
	ret.start = queue[0]->start;

	free(queue[0]);
	queue[0] = queue[next - 1];
	queue[next - 1] = NULL;
	next--;
	percDown(0);

	if(next < size / 2 && size > MIN_SIZE){
		//realloc
		struct qnode ** temp = realloc(queue, (size / 2) * sizeof(struct qnode *));
		if(temp != NULL){
			size /= 2;
			queue = temp;
		}
		else{
			ret.start = -1; //used to throw an error
			ret.end = -1;
		}
	}

	return ret;
}

#endif

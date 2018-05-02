#include "threads.h"

//thread_PrimeCalculate function to calculate the primes between the ranges which
//are stored into the threadArgs structure
void *thread_PrimeCalculate(void *threadId)
{
	threadArgs *args = threadId;

	pthread_mutex_lock( args->mutex );
	//critical section (below)
	int flag;

	for(int i = args->rNum1; i <= args->rNum2; i++) {

		flag = 0;
		for(int j = 2; j<=i/2; j++) {
			if( (i%j) == 0) {
				flag = 1;
				break;
			}
		}
		if(flag == 0 && i != 1) {
			insertArray(args->t_array, i);
		}
	}
	//end of critical section
	pthread_mutex_unlock( args->mutex );

	return 0;
}

//initArray function to initialize the array to the defined ISIZE
void initArray (Array *a, size_t initialSize) {
	a->arr = (long*)malloc(initialSize * sizeof(long));
	a->used = 0;
	a->size = initialSize;
}

//insertArray function which will insert each prime as it is calculated
//and will automatically resize if necessary
void insertArray(Array *a, long element) {

	if (a->used == a->size) {
		a->size *= 2;
		a->arr = (long*)realloc(a->arr, a->size * sizeof(long));
	}
	a->arr[a->used++] = element;
}

// function which will free the dynamic array within the tArgs
//structure
void clearArray (Array *a) {
	free(a->arr);
	a->arr = NULL;
	a->used = a->size = 0;
}
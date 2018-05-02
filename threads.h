#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	long* arr;
	size_t used;
	size_t size;
} Array;

//Crearted a struct for the threads. 
//This is done to allow the passing of multiple variables
// to the thread function
typedef struct {
	int tid,
		rNum1,
		rNum2;
	Array *t_array;
	pthread_mutex_t *mutex;
} threadArgs;

void *thread_PrimeCalculate(void *threadId);
void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, long element);
void clearArray(Array *a);
#endif
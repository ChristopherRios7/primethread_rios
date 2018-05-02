#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include "threads.h"

#define ISIZE 100 //initial size of array


typedef struct mesg_buffer {
	long mesg_type;
	char mesg_text[ISIZE];
} message;

typedef struct {
	pid_t pid;
	long R1, R2, Psum;
} childArgs; //child arguments

childArgs processWork(int tNum, pthread_mutex_t *mutex);
void tknMsg(char *msg, long *arr);
void prtnRange(long r1 , long r2, int count, long* Ranges, int SIZE);
long procSumPrimes(Array *ptr);

#endif
#include "process.h"

childArgs processWork(int tNum, pthread_mutex_t *mutex) {

	childArgs child;
	childArgs *c = &child;
	c->pid = getpid();

	message mrcv;
	key_t key;
	int msgid;
	long ranges[2];

	//ftok to generate unique key
	if ((key = ftok("msgq.txt", 70)) == -1) {
		perror("ftok");
		exit(1);
	}
	//msgget creates a message queue and returns identifier
	if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}

	if (msgrcv(msgid, &mrcv, sizeof(mrcv.mesg_text), 1, 0) == -1){
		perror("msgrcv");
		exit(1);
	}
	
    //tknMsg call sends the ranges to the msgqueue 
	tknMsg(mrcv.mesg_text, ranges);
	c->R1 = ranges[0];
	c->R2 = ranges[1];

	printf("c->minrang: %ld c->maxrang: %ld\n", c->R1, c->R2);

	//double the amount of threads since range is between two numbers
	int TRSIZE = tNum*2;
	long tRanges[TRSIZE];

	//partition the range of the process for each thread
	prtnRange(c->R1, c->R2, tNum, tRanges, TRSIZE);

	pthread_t threads[tNum];
	threadArgs args[tNum];

	//variable int y accumulater to traverse through ranges for each thread
	int y = 0;

	//for loop that will assign and initialize all the member variables within
	//threadArgs structures for each thread
	for (int i = 0; i<tNum; i++) {
		threadArgs *ptr = &args[i];
		ptr->tid = i+1;
		ptr->rNum1 = tRanges[y];
		ptr->rNum2 = tRanges[y+1];
		ptr->mutex = mutex;			//set mutex to address of key (pass mutex by reference)
		Array arr;
		ptr->t_array = &arr;
		initArray(ptr->t_array, ISIZE);

		if ( pthread_create(&threads[i], NULL, thread_PrimeCalculate, (void*)ptr) != 0) {
			perror("Error: Unable to create thread using pthread_create()");
			exit(errno);
		}
		y += 2;
	}

	//wait for threads to complete
	for(int j=0; j<tNum; j++) {
		pthread_join( threads[j], NULL );
	}

	//print process info from threads
	//printf("Process %ld\n", (long)c->pid);
	Array tPrimes;
	Array *tptr = &tPrimes;
	initArray(tptr, ISIZE);

	for(int k = 0; k < tNum; k++) {
		threadArgs *p = &args[k];
		for(unsigned m = 0; m < p->t_array->used; m++)
			insertArray(tptr, p->t_array->arr[m]);
		//deallocating memory
		clearArray(p->t_array);
	}
	
	//calculate and store the sum of the threads prime calculations
	c->Psum = procSumPrimes(tptr);
	//deallocating memory
	clearArray(tptr);

	printf("sum: %ld\nend of process %ld\n", c->Psum, (long)c->pid);
	return child;
}

//method to tokenize message of ranges from parent process and store them into
//an array of type long
void tknMsg(char *msg, long *arr)
{
	char *token, //char pointer to array of char to store string after using strtok function
		 *space = " \t\n\f\r\v"; //char pointer to array to function as delimiter

		 token = strtok(msg, space);
		 int i = 0;
		 while (token != NULL) {
		 	char *p = token;
		 	if(isdigit(*p))
		 		arr[i] = strtol(p, &p, 10);
		 	i++;
		 	token = strtok(NULL, space);
		 }
}

//function to partition the range among either processes or threads
void prtnRange(long r1 , long r2, int count, long* Ranges, int SIZE) {

	long rAmount = (r2 - r1)/count;//range num 2 minus range num 1 (difference between ranges)
	int flag; //flag for switch statement to handle uneven partitioning of ranges

	for(int i = 0; i < SIZE; i+=2) {
		if (r1 < r2)
		{
			Ranges[i] = r1;
			Ranges[i+1] = r1 + rAmount;
			flag = (Ranges[i+1] > r2)? 1:0;
			switch(flag)
			{
				case 0:
				r1 = Ranges[i+1] + 1;
				break;
				case 1:
				Ranges[i+1] = r2;
				break;
			}
		}
	}
}

//function to process the sum of the array passed to it. 
long procSumPrimes(Array *ptr) {
	long sum = 0;
	for(unsigned n = 0; n < ptr->used; n++) {
		sum += ptr->arr[n];
	}
	return sum;
}
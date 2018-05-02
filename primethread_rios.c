
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include "process.h"

#define SZ 50
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void prtProcSums(childArgs proc);
void prtPrimeTotal(int min, int max, long total);
void cpyStruct(childArgs *pInfo, childArgs *p);

int main(int argc, char *argv[])
{
	int range[2],
		processNum,
		threadNum;
	message message;
	key_t key;
	int msgid;
	system("touch msgq.txt");

	//ftok to generate unique key
	key = ftok("msgq.txt", 70);

	if (argc != 5)
	{
		printf("missing or too many command line argument(s)\n");
		exit(-1);
	} //end of if statement
	else
	{
		range[0] = atoi(argv[1]);
		range[1] = atoi(argv[2]);
		processNum = atoi(argv[3]);
		threadNum = atoi(argv[4]);

		//must be 2 times the amount of processes since each process needs to have
		//a range between two numbers
		int SIZE = processNum*2;
		//process ranges for each process
		long Ranges[SIZE];
		//partition the range input by the user to divide among each process
		prtnRange(range[0], range[1], processNum, Ranges, SIZE);

		//msgget creates a message queue and returns identifier
		if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1) {
			perror("msgget");
			exit(1);
		}
		message.mesg_type = 1;
		int x = 0;
		char range1[SZ],
			 range2[SZ];
		for (int n = 0; n < processNum; n++) {
			sprintf(range1, "%ld", Ranges[x]);
			sprintf(range2, "%ld", Ranges[x+1]);
			char *str = (char *) malloc(1 + strlen(range1) + strlen(range2));
			strcpy(str, range1);
			strcat(str," ");
			strcat(str, range2);
			strcpy(message.mesg_text, str);
			if ( msgsnd(msgid, &message, sizeof(message.mesg_text), 0) == -1) {
				perror("msgsnd");
				return EXIT_FAILURE;
			}
			x += 2;
		}

		pid_t pids[processNum];
		childArgs *psInfo[processNum]; //struct childArgs pointer array to point to each child process's structure

		childArgs pstructs[processNum];
		//accumulator used to traverse through the Ranges array
		for (int i = 0; i<processNum; i++) {
			if( (pids[i] = fork()) < 0 ) {
				perror("Error: fork()");
				exit(errno);
			}
			else if ( pids[i] == 0) {
				pstructs[i] = processWork(threadNum, &mutex);
				 childArgs *ptr = &pstructs[i];
				 printf("Process: %ld minrang: %ld maxrang: %ld Sum: %ld\n", (long)ptr->pid, ptr->R1, ptr->R2, ptr->Psum);
				exit(0);
			}
		} //end of for loop

		//wait for children to exit
		int status;
		//pid_t pid;
		int pNum = processNum;
		while (pNum > 0) {
			wait(&status);
			--pNum;
		}

		// to destroy the message queue
    	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
    		perror("msgctl");
    		exit(1);
    	}
		//destroy mutex since it will no longer be needed
		pthread_mutex_destroy(&mutex);

	} //end of else

	printf("end of main\n");
	return 0;
} //end of main

//method to print each process sum of the primes between their distinct ranges
void prtProcSums(childArgs proc) {
		childArgs *ptr = &proc;
		printf("The sum of the prime numbers computed by process %ld is: %ld\n", (long)ptr->pid, ptr->Psum);
}

//method to print the total of all the process prime sums
void prtPrimeTotal(int min, int max, long total) {
	printf("The total sum of the prime numbers in the range [%d %d] is: %ld\n", min, max, total);
}

//method to copy the process structures to pass process info to the parent process
void cpyStruct(childArgs *pInfo, childArgs *p) {
	pInfo->pid = p->pid;
	pInfo->R1 = p->R1;
	pInfo->R2 = p->R2;
	pInfo->Psum = p->Psum;
	printf("Process: %ld minrang: %ld maxrang: %ld Sum: %ld\n", (long)pInfo->pid, pInfo->R1, pInfo->R2, pInfo->Psum);
}
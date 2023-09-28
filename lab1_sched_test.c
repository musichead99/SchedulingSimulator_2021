/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 정성구
*	    Student name : 32164120
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

int main(int argc, char *argv[]){

	int n, i;

	printf("Number of Processes : ");
	scanf("%d", &n);

	ps psArr[n];
	printf("     AT ST\n");
	for (i = 0; i < n; i++) {
		printf("ps%d : ", i + 1);
		scanf("%d %d", &psArr[i].arrivedTime, &psArr[i].serviceTime);
		psArr[i].num = i;
	}

	FIFO_Scheduling(psArr, n);
	SJF_Scheduling(psArr, n);
	RR_Scheduling(psArr, n, 1);
	RR_Scheduling(psArr, n, 4);
	MLFQ_Scheduling(psArr, n, 1);
	MLFQ_Scheduling(psArr, n, 2);
	Lottery_Scheduling(psArr, n);

	return 0;
}


/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32164120
*	    Student name : 정성구
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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
#define MAXARR 20

/* 실행결과 출력 */

void printArr(int cArr[][MAXARR], int n){

	int i, j;
	printf("    0                                       20\n");
	printf("    | - - - - | - - - - | - - - - | - - - - |\n");

	for(i = 0; i < n; i++){
		printf("ps%d |", i + 1);
		for(j = 0; j < MAXARR; j++){
			if(cArr[i][j] == 1) printf("= ");
			else printf("  ");
		}
		printf("\n");
	}
	printf("\n");
}

/* Queue APIs */

void Queue_init(Queue *q) {
	q->front = NULL;
	q->rear = NULL;
}

int is_empty(Queue *q) {
	return (q->front == NULL);
}

void enqueue(Queue *q, ps data) {
	
	node *tmp = (node*)malloc(sizeof(node));
	tmp->data = data;
	tmp->next = NULL;
	if(is_empty(q)) {
		q->front = tmp;
		q->rear = tmp;
	}
	else {
		q->rear->next = tmp;
		q->rear = tmp;
	}
}

ps dequeue(Queue *q) {
	
	node *tmp = q->front;
	ps data = tmp->data;
	q->front = q->front->next;

	if(q->front == NULL)
		q->rear = NULL;

	free(tmp);
	return data;
}

/* Linked List APIs */

int list_is_empty(List* L) {
	return L->head->next == L->tail;
}

void list_init(List* L) {
	L->head = (node*)malloc(sizeof(node));
	L->tail = (node*)malloc(sizeof(node));

	L->head->next = L->tail;
	L->head->prev = NULL;
	L->tail->next = NULL;
	L->tail->prev = L->head;
}

void insert(List* L, ps data) {
	node *tmp = (node*)malloc(sizeof(node));
	tmp->data = data;
	
	L->tail->prev->next = tmp;
	tmp->prev = L->tail->prev;
	L->tail->prev = tmp;
	tmp->next = L->tail;
}

void ddelete(List* L,node *tmp) {
	
	tmp->next->prev = tmp->prev;
	tmp->prev->next = tmp->next;
	free(tmp);
}

/* Scheduling Policies */

int FIFO_Index(ps arr[], int n, int limit) {
	int i, tmp = 99, index = -1;

	for(i = 0; i < n; i++) {
		if(arr[i].runningTime && arr[i].arrivedTime <= limit && arr[i].arrivedTime < tmp) {
			tmp = arr[i].arrivedTime;
			index = i;
		}
	}
	return index;
}

void FIFO_Scheduling(ps arr[], int n){
	
	int i, j,index = -1;
	int cArr[n][MAXARR];

	/* 초기화 */
	for(i = 0; i < n; i++) {
		arr[i].runningTime = arr[i].serviceTime;
		memset(cArr[i], 0, sizeof(int)*MAXARR);
	}

	/* FIFO 알고리즘 */
	for(i = 0;i < MAXARR; i++){

		switch(index) {
			case -1:
				index = FIFO_Index(arr,n,i);
				if(index == -1) break; 
			default:
				if(arr[index].runningTime) {
					arr[index].runningTime--;
					cArr[index][i] = 1;
				}
				else {
					index = FIFO_Index(arr, n, i);
					i--;
				}

		}
	}
	printf("                     --FIFO--\n");
	printArr(cArr,n);
}

int SJF_Index(ps arr[], int n, int limit){

	int i, tmp = 99, index = -1;
	
	/* 현시점 가장 수행시간이 적은 프로세스를 찾음 */
	for(i = 0; i < n; i++) {
		if(arr[i].runningTime && arr[i].arrivedTime <= limit && tmp > arr[i].serviceTime){
			index = i;
			tmp = arr[i].serviceTime;
		}
	}
	return index;
}

void SJF_Scheduling(ps arr[], int n) { 

	int i, j, index = -1;
	int cArr[n][MAXARR];

	/* 초기화 */
	for(i = 0; i < n; i++) {
		arr[i].runningTime = arr[i].serviceTime;
		memset(cArr[i], 0, sizeof(int)*MAXARR);
	}
	
	/* SJF 알고리즘 */
	for(i = 0; i < MAXARR; i++){

		switch(index) {
			case -1:
				index = SJF_Index(arr,n,i);
				if(index == -1) break;
			default:
				if(arr[index].runningTime) {
					arr[index].runningTime--;
					cArr[index][i] = 1;
				}
				else {
					index = SJF_Index(arr,n,i);
					i--;
				}
		}
	}
	printf("                     --SJF--\n");
	printArr(cArr, n);
}

void RR_Scheduling(ps arr[], int n, int tq) {

	Queue RQ;
	Queue_init(&RQ);
	int i, j, k;
	int cArr[n][MAXARR];
	ps tmp;

	/* 초기화 */	
	for(i = 0; i < n; i++){
		arr[i].runningTime = arr[i].serviceTime;
		memset(cArr[i], 0, sizeof(int)*MAXARR);
		arr[i].flag = 0;
	}

	/* RR 알고리즘 */
	for(i = 0; i < MAXARR;) {
		
		/* 큐가 비어있지 않다면 */
		if(!is_empty(&RQ)) {

			tmp = dequeue(&RQ);
			for(k = 0; k < tq && tmp.runningTime; k++) {
				tmp.runningTime--;
				cArr[tmp.num][i] = 1;
				i++;
				for(j = 0; j < n; j++) {
					if(arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
						enqueue(&RQ, arr[j]);
						arr[j].flag = 1;
					}
				}
			}
			if(tmp.runningTime) enqueue(&RQ, tmp);
		}
		/* 큐가 비어있다면 */
		else {

			for(j = 0; j < n; j++) {
				if(arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
					enqueue(&RQ, arr[j]);
					arr[j].flag = 1;
				}
			}
			if(is_empty(&RQ)) i++;
		}
	}
	
	printf("               --RR (quantum = %d)--\n", tq);
	printArr(cArr, n);
}

void MLFQ_Scheduling(ps arr[], int n, int tq) {

	Queue Q0, Q1, Q2;
	Queue_init(&Q0);
	Queue_init(&Q1);
	Queue_init(&Q2);

	int i, j, k;
	int q2 = 1;
	int q1 = q2*tq;
	int q0 = q1*tq; 	
	int cArr[n][MAXARR];
	ps tmp;

	/* 초기화 */
	for(i = 0; i < n; i++) {
		arr[i].runningTime = arr[i].serviceTime;
		memset(cArr[i], 0, sizeof(int)*MAXARR);
		arr[i].flag = 0;
	}
	
	/* MFLQ 알고리즘 */
	for(i = 0; i < MAXARR;) {

		/* 큐가 모두 비어있다면 */
		if(is_empty(&Q2) && is_empty(&Q1) && is_empty(&Q0)) {
			for(j = 0; j < n; j++) {
				if(arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
					enqueue(&Q2, arr[j]);
					arr[j].flag = 1;
				}
			}
			if(is_empty(&Q2)) i++;
		}
		/* 큐가 하나라도 비어있지 않다면 */
		else {
			if(!is_empty(&Q2)) {

				tmp = dequeue(&Q2);
				for(k = 0; k < q2 && tmp.runningTime; k++) {
					tmp.runningTime--;
					cArr[tmp.num][i] = 1;
					i++;
					for(j = 0; j < n; j++) {
						if(arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
							enqueue(&Q2, arr[j]);
							arr[j].flag = 1;
						}
					}
				}
				if(is_empty(&Q2) && is_empty(&Q1) && is_empty(&Q0)) {
					if(tmp.runningTime) enqueue(&Q2, tmp);
				}
				else
					if(tmp.runningTime) enqueue(&Q1, tmp);
			}
			else if(!is_empty(&Q1)) {

				tmp = dequeue(&Q1);
				for (k = 0; k < q1 && tmp.runningTime; k++) {
					tmp.runningTime--;
					cArr[tmp.num][i] = 1;
					i++;
					for (j = 0; j < n; j++) {
						if (arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
							enqueue(&Q2, arr[j]);
							arr[j].flag = 1;
						}
					}
				}
				if (tmp.runningTime) enqueue(&Q0, tmp);
			}
			else if(!is_empty(&Q0)) {

				tmp = dequeue(&Q0);
				for (k = 0; k < q0 && tmp.runningTime; k++) {
					tmp.runningTime--;
					cArr[tmp.num][i] = 1;
					i++;
					for (j = 0; j < n; j++) {
						if (arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
							enqueue(&Q2, arr[j]);
							arr[j].flag = 1;
						}
					}
				}
				if (tmp.runningTime) enqueue(&Q0, tmp);
			}
		}
	}
	printf("             --MLFQ (quantum = %d^i)--\n", tq);
	printArr(cArr, n);
}

void Lottery_Scheduling(ps arr[], int n) {
	
	srand((unsigned int)time(NULL));
	int cArr[n][MAXARR], counter, winner;
	int i, j, ticket = 100;
	int CompletedJob = 0;
	List L;
	list_init(&L);
	node* tmp;

	/* 초기화 */
	for(i = 0; i < n; i++) {
		printf("ps%d's Ticket(%d left) : ", i + 1, ticket);
		scanf("%d", &arr[i].ticket);
		ticket -= arr[i].ticket;
		arr[i].runningTime = arr[i].serviceTime;
		arr[i].flag = 0;
		memset(cArr[i],0,sizeof(int)*MAXARR);
	}
	
	/* 티켓 오사용 검출 */
	if(ticket > 0) {
		printf("wrong usage : Please use up all your tickets\n");
		exit(1);
	}
	else if(ticket < 0) {
		printf("wrong usage : all of your tickets are 100\n");
		exit(1);
	}

	/* Lottery 알고리즘 */
	for(i = 0; i < MAXARR;) {
	
		/* 리스트가 비어있지 않다면 */
		if(!list_is_empty(&L)) {
			
			/* 난수 생성과 counter변수, tmp 초기화 */
			winner = rand() % ticket;
			counter = 0;
			tmp = L.head->next;
			
			while(tmp != L.tail) { 	 
				counter += tmp->data.ticket;
	
				if(counter > winner)
					break;
				tmp = tmp->next;
			}
			
			/* 배열에 수행결과 기록 */
			tmp->data.runningTime--;
			cArr[tmp->data.num][i] = 1;
			i++;

			/* 현 시점 도착한 프로세스가 있는지 검사 */
			for(j = 0; j < n; j++) {
				if(arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
					arr[j].flag = 1;
					insert(&L, arr[j]);
					ticket += arr[j].ticket; // 티켓 갱신
				}
			}

			/* 프로세스의 수행시간이 다 끝났다면 리스트에서 제거한다. */
			if(!tmp->data.runningTime) {
				ticket -= tmp->data.ticket; // 티켓 갱신
				ddelete(&L, tmp);				
			}
			
			
		}
		/* 리스트가 비어있다면 */
		else {
	
			/* 프로세스 배열에서 도착한 프로세스가 있는지 확인 */
			for(j = 0; j < n; j++) {
				if(arr[j].runningTime && arr[j].arrivedTime <= i && !arr[j].flag) {
					arr[j].flag = 1;
					insert(&L,arr[j]);
					ticket += arr[j].ticket; // 티켓 갱신
				}
			}
			/* 도착한 프로세스가 없다면 아무 프로세스도 실행하지 않음 */
			if(L.head->next == L.tail) i++;
		}
	}

	free(L.head);
	free(L.tail);

	printf("                   --Lottery--\n");
	printArr(cArr, n);
}

/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 정성구
*	    Student name : 32164120
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H

#define MAXARR 20

typedef struct ps{
	int arrivedTime;
	int serviceTime;
	int runningTime;
	int flag;
	int num;
	int ticket;	
}ps;

typedef struct node {
	ps data;
	struct node* next;
	struct node* prev;
}node;

typedef struct Queue {
	node *front, *rear;
}Queue;

typedef struct List {
	node *head, *tail;
}List;

/* Queue APIs */
void Queue_init(Queue *q);
int is_empty(Queue *q);
void enqueue(Queue *q, ps data);
ps dequeue(Queue *q);

/* Linked List APIs */
void list_init(List* L);
void insert(List* L, ps data);
void ddelete(List* L, node* tmp);
int list_is_empty(List* L);

void printArr(int cArr[][MAXARR],int n);

void FIFO_Scheduling(ps arr[], int n);
int SJF_Index(ps arr[], int n, int limit);
void SJF_Scheduling(ps arr[], int n);
void RR_Scheduling(ps arr[], int n, int q);
void MLFQ_Scheduling(ps arr[], int n, int q);
void Lottery_Scheduling(ps arr[], int n);

#endif /* LAB1_HEADER_H*/

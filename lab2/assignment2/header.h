#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h> 
#include <pthread.h>

#include <sys/time.h>

// assumption maximum line for input is 100
#define MAX_LINE_LEN 100
// stop signal is -100
#define STOP -100

extern int threadNum, workNum, askNum, receiveNum, completeNum, sleepNum;
extern pthread_mutex_t x_mutex;
extern pthread_cond_t q_avaliable_cond;
extern pthread_mutex_t q_mutex;
extern pthread_mutex_t job_mutex;
extern int isStop;

FILE* curFile;
struct timeval start, end;
double execTime;

struct Queue{
  int size, curSize;
  int *queueArray;
};



struct Queue* queue;
int *jobManager;

void Trans( int n );
void Sleep( int n );

void initQueue(int length);
bool isFull(struct Queue* queue);
bool isEmpty(struct Queue* queue);
bool enque(struct Queue* queue, int temp);
int dequeue(struct Queue* queue);
void stop(struct Queue* queue);

void* consumerWork(void *i);

#endif
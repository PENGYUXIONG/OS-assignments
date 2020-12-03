#include "header.h"

pthread_cond_t job_avaliable_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t q_avaliable_cond = PTHREAD_COND_INITIALIZER;

void initQueue(int length){
  queue = (struct Queue*) malloc(sizeof(struct Queue));
  queue -> size = length;
  queue -> curSize = 0;
  queue -> queueArray = (int*)malloc(queue -> size * sizeof(int) + 1);
}

bool isFull(struct Queue* queue){
  return (queue -> size == queue -> curSize);
}

bool isEmpty(struct Queue* queue){
  return (queue -> curSize == 0);
}

bool enque(struct Queue* queue, int temp){

  pthread_mutex_lock(&q_mutex);
  while (isFull(queue)){
    pthread_cond_wait(&q_avaliable_cond, &q_mutex);
  }
  queue -> queueArray[queue -> curSize] = temp;

  queue -> curSize++;
  // if not stop, notify next thread to continue its work
  if (temp != STOP){
    pthread_cond_signal(&job_avaliable_cond);
    
  }
  
  pthread_mutex_unlock(&q_mutex);
  pthread_cond_signal(&q_avaliable_cond);
  return true;
}

int dequeue(struct Queue* queue){
  int temp;
  pthread_mutex_lock(&q_mutex);
  while (isEmpty(queue)){
    pthread_cond_wait(&q_avaliable_cond, &q_mutex);
  }
  temp = queue->queueArray[0];
  for (int i = 0; i < queue->size; i++){
    queue->queueArray[i] = queue->queueArray[i+1];
  }

  queue -> curSize--;
  pthread_mutex_unlock(&q_mutex);
  pthread_cond_signal(&q_avaliable_cond);
  return temp;
  
}

// put stop sign into queue
void stop(struct Queue* queue){
  // push thread number of stops to make sure mo thread is missing the signal stop
  for (int i = 0; i < threadNum; i++){
    enque(queue, STOP);
    isStop++;
  }
  // wake up blocked threads
  pthread_cond_broadcast(&job_avaliable_cond);
}
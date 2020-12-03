#include "header.h"

void* consumerWork(void *i){
  
  int index = *((int *) i) + 1;
  free(i);
  char output[1024];
  while (true){
    // ask for work
    gettimeofday(&end, NULL);
    execTime = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec) / 1000000);
    pthread_mutex_lock(&x_mutex);
    sprintf(output, "%8.3f ID= %d      Ask\n", execTime, index);
    fputs(output, curFile);
    pthread_mutex_unlock(&x_mutex);
    askNum++;

    // receive work
    int temp = dequeue(queue);
    gettimeofday(&end, NULL);
    execTime = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec) / 1000000);
    if (temp == STOP){
      isStop--;
      break;
    }
    pthread_mutex_lock(&job_mutex);
    jobManager[index-1]++;
    pthread_mutex_unlock(&job_mutex);

    pthread_mutex_lock(&x_mutex);
    int queueSize = queue->curSize;
    if (isStop > 0){
      queueSize = queueSize - isStop;
    }
    sprintf(output, "%8.3f ID= %d Q= %d Receive %5d\n", execTime, index, queueSize, temp);
    fputs(output, curFile);
    pthread_mutex_unlock(&x_mutex);
    receiveNum++;

    // doing work
    Trans(temp);
    
    // complete work
    gettimeofday(&end, NULL);
    execTime = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec) / 1000000);
    pthread_mutex_lock(&x_mutex);
    sprintf(output, "%8.3f ID= %d      Complete %4d\n", execTime, index, temp);
    fputs(output, curFile);
    pthread_mutex_unlock(&x_mutex);
    completeNum++;
  }  
  
  return NULL;
}
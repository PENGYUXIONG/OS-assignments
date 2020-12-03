#include "header.h"

pthread_mutex_t x_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;


int threadNum = 0;
int workNum, askNum, receiveNum, completeNum, sleepNum = 0;
int isStop = 0;

char *parseOutputFileName(int argc, char** argv, int *threadNum){
  char *id = "0";
  if (argc < 2){
    printf("insufficient arguements, number of consumer threads is missing!\n");
    return NULL;
  }

  *threadNum = atoi(argv[1]);

  jobManager = (int*) malloc(*threadNum * sizeof(int));

  if (argc > 2){
    bool isValid = true;
    int i = 0;
    while(argv[2][i]){
      if (isdigit(argv[2][i]) == 0){
        isValid = false;
        break;
      }
      i++;
    }
    if (isValid){
      id = argv[2];
    }else{
      printf("invalid id for output file, id need to be a valid integer!\n");
      return NULL;
    }
  }

  int idLength = strlen(id);
  // assume output file name length is 13 char + id length. "prodcon.id.log\0"
  char *fileName = malloc(13 + idLength);
  if (strcmp(id, "0") == 0){
    fileName = "prodcon.log";
    
  }else{
    strcpy(fileName, "prodcon.");
    strcat(fileName , id);
    strcat(fileName, ".log");
  }
  return fileName;
}


int main(int argc, char *argv[]) {
  gettimeofday(&start, NULL);

  char *fileName = parseOutputFileName(argc, argv, &threadNum);
  int queueSize = 2 * threadNum;
  initQueue(queueSize);

  curFile = fopen(fileName, "w");

  pthread_t threads[threadNum];
  int *curThread;

  for (int i = 0; i < threadNum; i++){
    pthread_mutex_lock(&job_mutex);
    jobManager[i] = 0;
    pthread_mutex_unlock(&job_mutex);
  }

  // start threads 
  for (int i = 0; i < threadNum; i++){
    curThread = (int *) malloc(sizeof(int));
    *curThread = i;
    pthread_create(&threads[i], NULL, consumerWork, (void*) curThread);
  }

  char curLine[MAX_LINE_LEN];
  char output[1024];
  // read in and output
  while (fgets(curLine, MAX_LINE_LEN, stdin)){
    if (curLine[0] == 'T'){
      char jobCode[MAX_LINE_LEN];
      memmove(jobCode, curLine+1, strlen(curLine));
      // char jobCode = curLine[1];
      gettimeofday(&end, NULL);
      execTime = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec) / 1000000);
      enque(queue, atoi(jobCode));
      pthread_mutex_lock(&x_mutex);
      int queueSize = queue->curSize;
      if (isStop > 0){
        queueSize = queueSize - isStop;
      }
      sprintf(output, "%8.3f ID= 0 Q= %d Work %s\n", execTime, queueSize, jobCode);
      fputs(output, curFile);
      pthread_mutex_unlock(&x_mutex);
      workNum++;
    }

    else if (curLine[0] == 'S'){
      char jobCode[MAX_LINE_LEN];
      memmove(jobCode, curLine+1, strlen(curLine));
      // jobCode[0] = curLine[1];
      gettimeofday(&end, NULL);
      execTime = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec) / 1000000);
      pthread_mutex_lock(&x_mutex);
      sprintf(output, "%8.3f ID= 0    Sleep %s\n", execTime, jobCode);
      fputs(output, curFile);
      pthread_mutex_unlock(&x_mutex);

      Sleep(atoi(jobCode));
      sleepNum++;
    }

    else{
      printf("Unknown input");
      break;
    }
    workNum++;
  }

  // no more job
  stop(queue);
  pthread_mutex_lock(&x_mutex);
  sprintf(output, "%8.3f ID= 0      End\n", execTime);
  fputs(output, curFile);
  pthread_mutex_unlock(&x_mutex);

  // end thread
  for(int i = 0; i < threadNum; i++){
    pthread_join(threads[i], NULL);
  }

  free(queue->queueArray);
  free(queue);
  free(fileName);

  gettimeofday(&end, NULL);
  execTime = (double)(end.tv_sec - start.tv_sec) + ((double)(end.tv_usec - start.tv_usec) / 1000000);
  sprintf(output, "Summary:\n    Work %9d\n", workNum);
  fputs(output, curFile);
  sprintf(output, "    Ask %10d\n    Receive %6d\n", askNum, receiveNum);
  fputs(output, curFile);
  sprintf(output, "    Complete %5d\n    Sleep %8d\n", completeNum, sleepNum);
  fputs(output, curFile);

  for (int i = 0; i < threadNum; i++){
    sprintf(output, "    Thread %2d %4d\n", i+1, jobManager[i]);
    fputs(output, curFile);
  }

  sprintf(output, "Transactions per second: %.2f\n", completeNum / execTime);
  fputs(output, curFile);

  fclose(curFile);
  free(jobManager);
  return 0;
}
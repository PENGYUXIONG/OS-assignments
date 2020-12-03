#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#define LINE_LENGTH 100
#define MAX_ARGS 7
#define MAX_LENGTH 20
#define MAX_PT_ENTRIES 32
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>
#include <sys/resource.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


struct ProcessInfo{
    pid_t pid;
    char pName[LINE_LENGTH];
};

#ifdef DEFINE_I
int processNum = 0;
int suspendProcessNum = 0;
#else
extern int processNum;
extern int suspendProcessNum;
#endif

struct ProcessInfo pcb[MAX_PT_ENTRIES];
struct ProcessInfo suspendPCB[MAX_PT_ENTRIES];

void removeProcessFromPCB(pid_t pid);
void removeAllProcess();
void updateProcess();
void moveFrontProcess(pid_t pid);
void moveBackProcess(pid_t pid);

int readCMD(char *input);
void parseArgs(char *input, char *parsedInput[], int *argNum);
void getFileOpCode(char *args[], char *inputFile[], char *outputFile[]);
int getStatus(char *args[], int argNum);

#endif
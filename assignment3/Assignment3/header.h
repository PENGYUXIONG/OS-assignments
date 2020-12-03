#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED


#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> 
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

// assumption maximum line for input is 100
#define MAX_LINE_LEN 1024
#define TIME_OUT 30
#define MAX_MSG_SIZE 4096

struct timeval curTime;

void Trans( int n );
void Sleep( int n );
double getTime();

#endif

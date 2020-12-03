#include "header.h"

double timeStamp;
bool isStart = false;
double startTime, endTime;
int socket_desc, client_sock, c, read_size;
bool runningStatus = true; 
int transNum = 1;

int fds[255] = {0};
int fd, socketMsg = 0;
// init trans number
int transCount[255] = {0};
char clientName[255][MAX_LINE_LEN];
int clientIndex = 0;

int fdNum = 1;

struct sockaddr_in server, client;
char client_message[MAX_MSG_SIZE];
char temp[1024];
char *fileName = "Server";

fd_set readfds;

void initServer(int argc, char *argv[]){
  if (argc != 2){
		perror("need 1 argument");
    exit(-1);
  }
  char* port = argv[1];

  if (atoi(port) < 5000 || atoi(port) > 64000 ){
    perror("port number need to be between 5000 and 64000");
    exit(-1);
  }


  sprintf(temp, "Using port %s\n", port);
  fputs(temp, stdout);

  //Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket\n");
	}

  //Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons( atoi(port) );

  // allow the address to be reused
  int yes = 1;
  setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  //Bind
	if( bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return;
	}

  //Listen
	int listenRes = listen(socket_desc, 3);
  if (listenRes < 0){
    perror("server listen");
    exit(-1);
  }
}

int main(int argc, char *argv[]){
  initServer(argc, argv);
  fdNum = socket_desc;

  while (runningStatus){

    struct timeval tv;
    tv.tv_sec = TIME_OUT;
    tv.tv_usec = 0;

    // init fds
    FD_ZERO(&readfds);
    // set server fd into the set   
    FD_SET(socket_desc, &readfds);

    for (int i = 0; i < fdNum; i++){
      fd = fds[i];
      if (fd > 0){
        FD_SET( fd , &readfds);
      }
      if (fd > fdNum){
        fdNum = fd;
      }
    }


    // set time out while checking fds
    socketMsg = select(fdNum + 1 , &readfds , NULL , NULL , &tv);   

    if (socketMsg < 0 && errno!=EINTR){
      perror("Select error");
      break;
    }

    else if (socketMsg == 0){
      // printf("client not requesting information, Server time out \n");
      break;
    }
    
    // if new conneciton
    if (FD_ISSET(socket_desc, &readfds)){
       //Accept and incoming connection
          c = sizeof(struct sockaddr_in);

          //accept connection from an incoming client
          client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
          if (client_sock < 0)
          {
            perror("accept failed");
            runningStatus = false;
            break;
          }

          // the connection is stable, store current file descriptor 
          fds[fdNum] = client_sock;
          fdNum++;
    }

    // iterate through existing descriptors
    // maintaining FIFO order
    for (int i = 0; i < fdNum; i++){
      fd = fds[i];
      // is visited socket, validatedstable connection
      if (FD_ISSET( fd , &readfds)){
        memset(client_message, 0, MAX_MSG_SIZE);
        char* clientHost;
        char* jobCode;

        read_size = recv(fds[i], client_message, MAX_MSG_SIZE, 0);
        if (read_size < 0){
          perror("read fail");
          break;
        }
        // client disconnect
        else if (read_size == 0){
          close(fds[i]);
          for (int j = i; j < fdNum; j++){
            fds[j] = fds[j+1];
          }
          fdNum--;
          i--;
          continue;
        }
        else{
          clientHost = strtok(client_message, "|");
          jobCode = strtok(NULL, "|");

          timeStamp = getTime();
          if (!isStart){
            startTime = timeStamp;
            isStart = true;
          }
          sprintf(temp, "%.2f: #    %d (T %s) from %s\n", timeStamp, transNum, jobCode, clientHost);
          fputs(temp, stdout);
          Trans(atoi(jobCode));
          timeStamp = getTime();
          sprintf(temp, "%.2f: #    %d (Done) from %s\n", timeStamp, transNum, clientHost);
          transNum++;
          fputs(temp, stdout);

          //Send the message back to client
          if( write(fds[i], jobCode, strlen(jobCode)) < 0){
            perror("failed to reply to client");
            break;
          }



          int curIndex = -1;
          // check whether it is old fd
          for (int k = 0; k < clientIndex; k++){
            if (strcmp(clientName[k], clientHost)==0){
              curIndex = k;
              break;
            }
          }
          // new fd
          if (curIndex == -1){
            curIndex = clientIndex;
            clientIndex++;
          }
          // job count +1 for current client
          memcpy(clientName[curIndex], clientHost, strlen(clientHost));
          transCount[curIndex]++;

          memset(client_message, 0, MAX_MSG_SIZE);
        }
      }
    }
  }
  

  // clear all fds
  for (int i = 0; i < fdNum; i++){
    if (fds[i] != 0){
      close(fds[i]);
    }
  }

  fputs("SUMMARY\n", stdout);
  int transTotal = 0;
  for (int i = 0; i < clientIndex; i++){
    transTotal = transTotal + transCount[i];
    sprintf(temp, "    %d transactions from %s\n", transCount[i], clientName[i]);
    fputs(temp, stdout);
  }
  endTime = timeStamp;
  double totalTime = endTime - startTime;
  double transSpeed = transTotal / totalTime;
  sprintf(temp, "%.1f transactions/sec (%d/%.2f)\n", transSpeed, transTotal, totalTime);
  fputs(temp, stdout);
  return 0;
}
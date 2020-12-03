#include "header.h"

int main(int argc, char *argv[]){
  if (argc != 3){
		perror("need 2 arguments");
    return 1;
  }
	FILE* curFile;
	double timeStamp;
  char* port = argv[1];
  char* ip_address = argv[2];
	char fileName[1024];
	int transNum = 0;

	if (atoi(port) < 5000 || atoi(port) > 64000 ){
    perror("port number need to be between 5000 and 64000");
    exit(-1);
  }

	int hostCode = gethostname(fileName, sizeof(fileName)); 
  if (hostCode == -1){
		perror("unable to get hostname");
	}

	pid_t curPid = getpid();
	char temp[1024];

	sprintf(temp, ".%d", curPid);
	strcat(fileName, temp);

	// start writing into the file
	curFile = fopen(fileName, "w");
	sprintf(temp, "Using port %s \n", port);
	fputs(temp, curFile);
	sprintf(temp, "Using server address %s \n", ip_address);
	fputs(temp, curFile);
	sprintf(temp, "Host %s \n", fileName);
	fputs(temp, curFile);

  int sock;
	struct sockaddr_in server;
	char server_reply[MAX_MSG_SIZE];

  // //Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}

	// unify the address msg
  server.sin_addr.s_addr = inet_addr(ip_address);
	server.sin_family = AF_INET;
	server.sin_port = htons( atoi(port) );


  // //Connect to remote server
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
  // //keep communicating with server
	char curLine[MAX_LINE_LEN];
	char jobCode[MAX_LINE_LEN];

	while (fgets(curLine, MAX_LINE_LEN, stdin)){
		int sendStatus = -1;
		if (curLine[0] == 'T'){
			transNum++;
			timeStamp = getTime();
			memmove(jobCode, curLine+1, strlen(curLine));
			strtok(jobCode, "\n");
			sprintf(temp, "%.2f: Send (%c %s) \n", timeStamp, curLine[0], jobCode);
			fputs(temp, curFile);

			
			strcpy(temp, fileName);
			char* msg = strcat(temp, "|");
			strcat(msg, jobCode);
			//Send some data
			sendStatus = send(sock, msg, strlen(msg), 0);
			if(sendStatus < 0)
			{
				puts("Send failed");
				return 1;
			}
			// sprintf(temp, "%d\n", sendStatus);
			// puts(temp);
		}

		else if (curLine[0] == 'S'){
			memmove(jobCode, curLine+1, strlen(curLine));
			strtok(jobCode, "\n");
			Sleep(atoi(jobCode));
			sprintf(temp, "Sleep %s units \n", jobCode);
			fputs(temp, curFile);
		}
		// shut down client with "kill" input
		else if (strcmp(curLine, "kill\n") == 0){
			break;
		}
		
		if (sendStatus > 0){
			//Receive a reply from the server
			memset(server_reply, 0, MAX_MSG_SIZE);
			int replyStatus = recv(sock, server_reply, MAX_MSG_SIZE, 0);
			// sprintf(temp, "%d\n", replyStatus);
			// puts(temp);
			if( replyStatus < 0){
				puts("recv failed");
				break;
			}
			else if (replyStatus > 0){
				timeStamp = getTime();
				sprintf(temp, "%.2f: Recv (D %s) \n", timeStamp, server_reply);
				fputs(temp, curFile);
			}
			else{
				printf("Server is down. Please try later \n");
				break;
			}
		}
	}

	close(sock);

	sprintf(temp, "Sent %d transactions \n", transNum);
	fputs(temp, curFile);

	fclose(curFile);
	return 0;
}
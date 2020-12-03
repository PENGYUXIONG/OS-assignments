/*
	C ECHO client example using sockets
*/
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[1000], server_reply[1000];
	
	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	// server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	inet_pton(AF_INET, "129.128.29.32", &(server.sin_addr));
	printf("%u\n", server.sin_addr.s_addr);
	char str[1000];
	inet_ntop(AF_INET, &(server.sin_addr), str, INET_ADDRSTRLEN);
	printf("%s\n", str);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");
	
	//keep communicating with server
	while(1)
	{
		memset(message, 0, 1000);
		printf("Enter message : ");
		scanf("%s", message);
		
		//Send some data
		puts("Server send :");
		puts(message);
		if( send(sock, message, strlen(message), 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		//Receive a reply from the server
		memset(server_reply, 0, 1000);
		if( recv(sock, server_reply, 1000, 0) < 0)
		{
			puts("recv failed");
			break;
		}
		puts("Server reply: ");
		puts(server_reply);
	}
	
	close(sock);
	return 0;
}

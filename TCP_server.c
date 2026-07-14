/* tcpserver.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
 if (argc != 3) {
	printf("Usage: %s <IP address> <port>\n", argv[0]);
        exit(1);
 }

 char *ipAddress = argv[1];
 int port = atoi(argv[2]);

 int sockfd;
 struct sockaddr_in serverAddr;
 
 int newSocket;
 struct sockaddr_in newAddr;
 
 socklen_t addr_size;
 char buf[1024];
 
 sockfd=socket(PF_INET,SOCK_STREAM,0);
 printf("Server socket Created Successfully...\n"); 
 memset(&serverAddr,'\0',sizeof(serverAddr));
 
 
 serverAddr.sin_family=AF_INET;
 serverAddr.sin_port=htons(port);
 serverAddr.sin_addr.s_addr=inet_addr(ipAddress);
 
 if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
     perror("bind failed");
        exit(1);
 }
 printf("Bind to Port Number %d\n", port);
 
 listen(sockfd,6);
 printf("Listening...\n");
 
 signal(SIGCHLD, SIG_IGN);
 
 while (1) {
    addr_size = sizeof(newAddr);
    newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
    if (newSocket < 0) {
        perror("accept failed");
        continue;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        close(newSocket);
        continue;
    }

    if (pid == 0) {
        // Child process: handles this client
        close(sockfd); // child doesn't need the listening socket
        
        printf("Client connected: IP = %s, Port = %d\n",
        		inet_ntoa(newAddr.sin_addr),
        		ntohs(newAddr.sin_port));
        printf("Client ID (PID): %d\n", getpid());

        strcpy(buf, "HELLO FROM ME\n");
        send(newSocket, buf, strlen(buf), 0);
        
        bool connected = true;
	while (connected) {
	   int bytesReceived = recv(newSocket, buf, 1024, 0);
 	   if (bytesReceived == 0) {
           	 printf("Client disconnected: IP = %s, Port = %d\n",
                 		  inet_ntoa(newAddr.sin_addr),
                		   ntohs(newAddr.sin_port));
           	 connected = false;
	    }else if (bytesReceived < 0) {
           	 perror("recv error");
           	 connected = false;
	    }else {

      	  	buf[bytesReceived] = '\0';  
     		printf("Data Received: %s\n", buf);
        	printf("Data Length: %d\n", bytesReceived);
    		}
	    	
	}

        printf("Closing Connection for a client...\n");
        close(newSocket);
        exit(0);
    } else {
        // Parent process: goes back to accept() for next client
        close(newSocket); // parent doesn't need the connected socket
    }
 }

 close(sockfd);
 
 return 0 ;
 
}

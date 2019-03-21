#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <netinet/in.h>


#define PORT 8080
#define MAX_MSG_LEN 256
#define RESPONSE_BYTES 512
#define REQUEST_BYTES 512

char* readFromFile(int fp) {
	long sz = lseek(fp, 0, SEEK_END);	
	lseek(fp, 0, SEEK_SET);
	if(sz == 0)
		return NULL;
	char *str = (char *)malloc((sz+1)*sizeof(char)); //+1 as lseek returns the number of bytes skipped as we need to consider the first one too
	read(fp, str,sizeof(char)*sz);
	str[sz] = 0;
    //printf("szvalue %ld\n",sz);
    //msg(str);msg("..........\n");
	return str;
}


void msg(char *str) {
	printf("%s", str);
}

char* receiveMsgFromServer(int sockFD) {
	int numPacketsToReceive = 0;
	int n = read(sockFD, &numPacketsToReceive, sizeof(int));
	if(n <= 0) {
		shutdown(sockFD, SHUT_WR);
		return NULL;
	}
	char *str = (char*)malloc(numPacketsToReceive*RESPONSE_BYTES);
	memset(str, 0, numPacketsToReceive*RESPONSE_BYTES);
	char *str_p = str;
	int i;
	for(i = 0; i < numPacketsToReceive; ++i) {
		int n = read(sockFD, str, RESPONSE_BYTES);
		str = str+RESPONSE_BYTES;
	}
	return str_p;
}

void sendMsgToServer(int sockFD, char *str) {
	int numPacketsToSend = (strlen(str)-1)/REQUEST_BYTES + 1;
	int n = write(sockFD, &numPacketsToSend, sizeof(int));
	char *msgToSend = (char*)malloc(numPacketsToSend*REQUEST_BYTES);
	strcpy(msgToSend, str);
	int i;
	for(i = 0; i < numPacketsToSend; ++i) {
		int n = write(sockFD, msgToSend, REQUEST_BYTES);
		msgToSend += REQUEST_BYTES;
	}
}

void updateUserTransFile(char *fileName, char *val) {
	//int fp = open(fileName, O_CREAT|O_RDONLY,0744);
	int fp = open(fileName, O_CREAT|O_RDONLY,0744);
	char *strbefore = readFromFile(fp);
	
	if(strbefore == NULL) {
		strbefore = "";
	}
	char *str = val;
	
	time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
	char *data = (char*)malloc((1 + strlen(asctime(localtime(&ltime))) + 1000 + strlen(str)+strlen(strbefore))*sizeof(char));
	sprintf(data, "%.*s,%s\n%s", (int)strlen(asctime(localtime(&ltime)))-1, asctime(localtime(&ltime)), val,strbefore);

	fp = open(fileName, O_CREAT|O_WRONLY);
	write(fp,data, strlen(data)*sizeof(char));
}
  


int main(int argc, char const *argv[])
{
    int connectedsocket = 0, n;
    struct sockaddr_in sadd,cadd;
	char recvbuff[1024];
	char *buff;
    char *msgFromServer;
    char msgToServer[MAX_MSG_LEN];
	memset(recvbuff,'0',sizeof(recvbuff));
    if ((connectedsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&sadd, '0', sizeof(sadd));
  
    sadd.sin_family = AF_INET;
    sadd.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, argv[1], &sadd.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(connectedsocket, (struct sockaddr *)&sadd, sizeof(sadd)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }

    /*
    //version 1
    //send(connectedsocket , hello , strlen(hello) , 0 );
    msgFromServer = receiveMsgFromServer(connectedsocket);msg(msgFromServer);
    memset(msgToServer, 0, sizeof(msgToServer));
    scanf("%s", msgToServer);
	sendMsgToServer(connectedsocket, msgToServer);
    //valread = read( connectedsocket , buffer, 1024);
    msgFromServer = receiveMsgFromServer(connectedsocket);msg(msgFromServer);
    msg("\n");
    return 0;
    */

    //version 2
	while(1){
		memset(msgToServer, 0, sizeof(msgToServer));
		msg("Client: ");//scanf(" %[^\n]", msgToServer);
		clock_t t = clock();
		usleep(2000000);
		sendMsgToServer(connectedsocket, "Sensor");
		msgFromServer = receiveMsgFromServer(connectedsocket);
		t = clock() - t;
		//printf (">>> RTT is : %f seconds\n\n\n", ((double)t)/CLOCKS_PER_SEC );
		if(msgFromServer == NULL)
			break;
		if(strcmp(msgFromServer,"Server shutting down...")==0){
			msg("Write end closed by the server.\n");
			shutdown(connectedsocket, SHUT_RD);
			msg("Connection closed by the server.\n");
			break;
		}
		msg("Server: ");msg(msgFromServer);msg("\n");
		updateUserTransFile("result.txt",msgFromServer);
	}
	    
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<sys/resource.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080

#define RESPONSE_BYTES 512
#define REQUEST_BYTES 512

#define USER 0
#define JOINT 1
#define ADMIN 2
#define UNAUTH_USER -1

#define MAX_DATA_SIZE 256
#define MAX_LEN 256
#define MAX_LINES_IN_MS 5
#define MAX_USERID_LEN 256
#define MAX_PASS_LEN 256

#define CREDIT 10
#define DEBIT 11
#define PRINT 12

struct sembuf p[1],v[1]; //these are semaphore ops - two for each semaphore (a p and a v)
int semid;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
}arg;

int file_exist (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}


void msg(char *str) {
	printf("%s", str);
}

void sendMsgToClient(int new_socket, char *str) {
	int numPacketsToSend = (strlen(str)-1)/RESPONSE_BYTES + 1;
	int n = write(new_socket, &numPacketsToSend, sizeof(int));
	char *msgToSend = (char*)malloc(numPacketsToSend*RESPONSE_BYTES);
	strcpy(msgToSend, str);
	int i;
	for(i = 0; i < numPacketsToSend; ++i) {
		int n = write(new_socket, msgToSend, RESPONSE_BYTES);
		msgToSend += RESPONSE_BYTES;
	}
}

char* receiveMsgFromClient(int new_socket) {
	int numPacketsToReceive = 0;
	int n = read(new_socket, &numPacketsToReceive, sizeof(int));
	if(n <= 0) {
		shutdown(new_socket, SHUT_WR);
		return NULL;
	}
    char *str = (char*)malloc(numPacketsToReceive*REQUEST_BYTES);
    memset(str, 0, numPacketsToReceive*REQUEST_BYTES);
    char *str_p = str;
    int i;
    for(i = 0; i < numPacketsToReceive; ++i) {
        int n = read(new_socket, str, REQUEST_BYTES);
        str = str+REQUEST_BYTES;
    }
    return str_p;
}

void closeWithMsg(char *str, int clientFD) {
	sendMsgToClient(clientFD, str);
	shutdown(clientFD, SHUT_RDWR);
}

void addStrings(char** str1, const char* str2,char del)
{
    size_t len1 = *str1 ? strlen(*str1) : 0;
    size_t len2 = str2 ? strlen(str2) : 0;
    char *res = realloc(*str1, len1 + len2 + 2);
    if (res)
    {
        res[len1] = del;
        memcpy(res + len1 + 1, str2, len2);
        res[len1 + 1 + len2] = 0;
        *str1 = res;
    }
}


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


int checkIfUserExists(char *userName) {
	int fp = open("database.txt", O_RDONLY);
	char delim[] = ", \n";
	char *str = readFromFile(fp);

	char *save_ptr;
	char *tok = strtok_r(str, delim, &save_ptr);
	do {
		int check = 0;
		if(tok!=NULL && strcmp(tok, userName) == 0) {
			check = 1;
		}
		tok = strtok_r(NULL, delim, &save_ptr);
		tok = strtok_r(NULL, delim, &save_ptr);
		if(strcmp(tok, "U") == USER && check == 1)
			return 1;
		tok = NULL;
	} while((tok = strtok_r(NULL, delim, &save_ptr)) != NULL);
	if(str!=NULL)
		free(str);
	return 0;
}



int isANumber(char *num) {
	int i = 0;
	int check = 0;
	for(i = 0; i < strlen(num); ++i) {
		if(isdigit(num[i])) {
			continue;
		}
		else if(num[i] == '.' && check == 0) {
			check = 1;
		}
		else {
			return 0;
		}
	}
	return 1;
}



char* checkCommand(char *command) {
	char delim[] = " ";
	char *str = command;
	char *filecontents;
	char *save_ptr;
	char *tok = strtok_r(str, delim, &save_ptr);
	int check = 0;
	if(strcmp(tok,"GET")==0){
		tok = NULL;
		check = 1;
		tok = strtok_r(NULL, delim, &save_ptr);
		if(file_exist(tok)){
			check = 2;
			int fp = open(tok, O_RDONLY);
			filecontents = readFromFile(fp);
			//msg(filecontents);
		}
		else{filecontents = "Doesn't exist!";}//msg(filecontents);}
	}
	else{check = 0;filecontents="Invalid Command! Use <GET>!";}//msg(filecontents);}
	return filecontents;
}

	




void talkToClient(int connectedsocket) {
	char *buff;
	char buffer[1024] = {0};
    
	while(1){
		buff = receiveMsgFromClient(connectedsocket);
			//printf("am i herer?\n");
		if(strcmp(buff,"Q")==0){
			msg("Server shutting down the client...\n");
			sendMsgToClient(connectedsocket, "Server shutting down...");
			closeWithMsg("Server shutting down the client...",connectedsocket);
			break;
		}

		printf("%d ",connectedsocket);msg("Client: ");msg(buff);msg("\n");
		memset(buffer,0,sizeof(buffer));

		char *checkCom = checkCommand(buff);
		sendMsgToClient(connectedsocket,checkCom);
		
		/*int checkFile = 0;
		if(checkCom==0){
			sendMsgToClient(connectedsocket,"Invalid command! Use <GET>!");
		}
		else if(checkCom==1){
			sendMsgToClient(connectedsocket,"File Doesn't exist!");
		}
		else if(checkCom==2){
			sendMsgToClient(connectedsocket,"File Exists");
		}*/
	}
	
}

int main(int argc, char const *argv[])
{
	//define the operations
	p[0].sem_num = 0; //operate on the first semaphore
	p[0].sem_op = -1; //operation is "substract one"
	p[0].sem_flg = 0; //other flags=0
	v[0].sem_num = 0; //operate on the first semaphore
	v[0].sem_op = 1; //operation is "add one"
	v[0].sem_flg = 0; //other flags=0
	//semaphore initializations
	int key;
    key = ftok(".",'a');
    semid = semget(key,1,IPC_CREAT|IPC_EXCL|0744); //how many semaphores you want
    arg.val = 1; //for binary
    semctl(semid,0,SETVAL,arg);
	//semaphore initializations
	
    int listensocket, new_socket, valread;
    struct sockaddr_in sadd,cadd;
    int opt = 1;
    int addrlen = sizeof(sadd);
    int caddrlen = sizeof(cadd);
    char *hello = "Hello from server";
      
    // Creating socket file descriptor
    if ((listensocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    sadd.sin_family = AF_INET;
    sadd.sin_addr.s_addr = INADDR_ANY;
    sadd.sin_port = htons( PORT );
      
    // Forcefully attaching socket to the port 8080
    if (bind(listensocket, (struct sockaddr *)&sadd,sizeof(sadd))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
	setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));


    if (listen(listensocket, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int csize;
    csize = sizeof(cadd); 
    
    int connectedsocket;
    while(1){
        memset(&cadd,0,sizeof(cadd));
		if((connectedsocket = accept(listensocket,(struct sockaddr*)&cadd,&csize))<0){
			error("Error on accept!\n");
		}
		
		switch (fork()){
			case -1:
				msg("Fork error!\n");
				break;
			case 0:
				close(listensocket);
				talkToClient(connectedsocket);
			default:
				close(connectedsocket);
				break;
		}
    }    
}

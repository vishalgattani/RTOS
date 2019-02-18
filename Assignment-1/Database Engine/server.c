#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
 
#define SERVER 1L
#define MAX_LINES_IN_MS 5

void printMsg(char *str) {
	printf("%s", str);
}

typedef struct {
    long    msg_to;
    long    msg_fm;
    char    buffer[BUFSIZ];
    
} MESSAGE;
 
int mid;
key_t key;
struct msqid_ds buf;
MESSAGE msg;

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



int file_exist (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

char* read_filename(char str[]) {
  
	char resp[200];
	char * split_text;
	/** Extracting First Word **/
	split_text = strtok (str," ,.-");
	if(strcmp(split_text,"GET")==0){
		/**Extracting file name **/
		split_text = strtok (NULL," ,-");
		int check = file_exist(split_text);
		return split_text;
	}
	
}






 
int main(int argc, char** argv) {
 
    //Creating a message queue
    key = ftok(".", 'z');
    if((mid = msgget(key, IPC_CREAT | 0660))<0){
        printf("Error Creating Message Queue\n");
        exit(-1);
    }
 
    //Display Message Queue and Server ID
    printf("Message Queue ID: %d\n", mid);
    printf("Server ID: %ld\n", (long)getpid());    
 
 	while(1){
	    //Receiving message from client, throws and error if input is invalid
	    if(msgrcv(mid, &msg, sizeof(msg.buffer), SERVER, 0) >= 0){
	    	//Server displays received message
	    	printf("==============================================\n");
	    	printf("%ld SERVER receives: ",msg.msg_fm);
	 		printf("%s\n", msg.buffer);
	 		//Aquiring Cliend PID to message return
		    long client = msg.msg_fm;
		 	char *filename = read_filename(msg.buffer);
		 	//printMsg(msg.buffer);
		    if(file_exist(filename)) {
		    	int fp = open(filename, O_RDONLY);
				char delim[] = ", \n";
				char *str = readFromFile(fp);
		    	strcpy(msg.buffer,str);
		    }
		    else strcpy(msg.buffer,"Invalid op/File Doesn't Exist");
		    //prep return message
		    msg.msg_fm = SERVER;
		    msg.msg_to = client;
		 	//send converting message back to client, throws and error if input is invalid
		    msgsnd(mid, (struct MESSAGE*)&msg, sizeof(msg.buffer), IPC_NOWAIT);
   	    	printf("==============================================\n\n");

		    }

	    }
	 
	    
	 
	    
    //server exits
    return (EXIT_SUCCESS);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
 
#define SERVER 1L
typedef struct {
    long    msg_to;
    long    msg_fm;
    char    buffer[BUFSIZ];
} MESSAGE;
 
int mid;
key_t key;
struct msqid_ds buf;
MESSAGE msg;


int file_exist (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

int read_contents(char str[], char response[]) {
  
  char resp[200];
  char * split_text;
  int fd_read;
  /** Extracting First Word **/
  split_text = strtok (str," ,.-");
  if(strcmp(split_text,"GET")==0){
  	/**Extracting file name **/
  	split_text = strtok (NULL," ,-");
	int check = file_exist(split_text);
	if(check)
  }
  else{
  	return -1;
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
	    	printf("SERVER receives:\n");
	 		printf("%s\n", msg.buffer);
	 	   //Aquiring Cliend PID to message return
		    long client = msg.msg_fm;
		 
		    //convert all lowercase characters to uppercase
		    int i=0;
		    while(msg.buffer[i] != '\0'){
		       if( msg.buffer[i] >= 'a' &&  msg.buffer[i] <= 'z'){
		       		msg.buffer[i]&='_';
		       }
		       else if( msg.buffer[i] >= 'A' &&  msg.buffer[i] <= 'Z'){
		       		msg.buffer[i]|=' ';
		       }
		       i++;
		    }
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
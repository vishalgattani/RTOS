#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <fcntl.h>

#define length 128
int main(int argc, char const *argv[])
{
	int cli_sock;
	struct sockaddr_in serv_add;
	char buff[1024];
	struct timeval start, end;
	char buffer[1024];
	struct flock fl = { F_WRLCK, SEEK_SET, 0, 0, 0 };
	
    printf("Creating the socket\n");
	if((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket error");
		exit(1);
	}

	// Setting server address
	printf("Setting server address\n");
	memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_add.sin_port = htons(8080);

    // Sending the connection request to server
	printf("Waiting for connection\n");
    if((connect(cli_sock, (struct sockaddr *)&serv_add, sizeof(serv_add))) < 0){
    	perror("Connection failed");
    	exit(1);
    }
    printf("Connection established\n\n");

    char path[] = "sensor_dat.txt";
    while(1){
    	strcpy(buffer, "1 ");
        int fd;
    	
    	if ((fd = open(path, O_RDONLY)) == -1) {
    		perror("open");
    		exit(1);
    	}
    	
    	if (flock(fd, 2) ) {
    		perror("fcntl");
    		exit(1);
    	}
    	
    	char message[length] = "";  
    	 
    	if(!read(fd,message, sizeof(message))) {
    		exit(1);
    	}	

    	strcat(message,"\0");
   		
   		fl.l_type = F_UNLCK;  /* set to unlock same region */
    
    	if (flock(fd, 8)) {
    		perror("fcntl");
    		exit(1);
    	}
    	
    	close(fd);
   		// Sending the data back
  
        strcat(buffer ,message);
        printf("%s\n", buffer);        
        // Sending the data back to client
        write(cli_sock, buffer, strlen(buffer));
        printf("Data sent\n");
        read(cli_sock, buff, sizeof(buff));
        sleep(2);
    }
    
    return 0;
}
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	int server_socket, new_socket;
    struct sockaddr_in serv_add, client_add;
    int addrlen = sizeof(serv_add);
    char buff[1024];
    char *buffer;
    FILE *csv_file;
    char *filename = "sensor_recv.txt";
    int data[2];  
    data[0]=0;
    data[1]=0;

    printf("Creating the socket\n");
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket error");
        exit(1);
    }

    //Setting up the address 
    memset(&serv_add, '\0', sizeof(serv_add));  
    serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = INADDR_ANY;
    serv_add.sin_port = htons(8080);

    // Binding server address to the socket
    printf("binding\n");  
    if((bind(server_socket, (struct sockaddr *)&serv_add,sizeof(serv_add))) < 0){
        perror("Bind error");
        exit(1);
    }

    // Listening to clients
    printf("listening\n");
    if(listen(server_socket, 5) < 0){
        perror("Listen error");
        exit(1);
    }

    while(1){
        // Accepting connections from client
        printf("Waiting for connection with client\n");
        if((new_socket = accept(server_socket, (struct sockaddr *)&serv_add, (socklen_t*)&addrlen)) < 0){
            perror("Accept error");
            exit(1);
        }
        printf("Request accepted\n");

        // Creating child process for any new client request
        if(!fork()){
            // Closing the server socket
            strcpy(buff,"");
            close(server_socket);
            // Message sending and receiving
            while(1){
                // Receing data from client
                printf("Start: %d %d\n",data[0], data[1]);
                printf("\nReceiving request message from client\n");
                read(new_socket, buff, sizeof(buff));
                // printf("%s\n",buff);
                int i = 0;
                
                if(buff[0] == '1')
                {	
                    // printf("ClientA\n");
		            for (i=2;i<1024;i++)
                		buff[i-2] = buff[i];
                	// printf("%s\n",buff);
					data[0]  = atoi(buff);	
                    FILE *f1 = fopen("Sensor1.txt","w");
                    fprintf(f1,"%d\n", data[0]);
                    fclose(f1);
					printf("%d %d\n",data[0], data[1]);	            
		        }
		        else if (buff[0] == '2')
		        {
                    // printf("ClientB\n");
		        	for (i=2;i<1024;i++)
                		buff[i-2] = buff[i];
                	// printf("%s\n",buff);
		            data[1]  = atoi(buff);
                    FILE *f2 = fopen("Sensor2.txt","w");
                    fprintf(f2,"%d\n", data[1]);
                    fclose(f2);
		            printf("%d %d\n",data[0], data[1]);
		        }   

                FILE *f1 = fopen("Sensor1.txt","r");
                fscanf(f1, "%d\n", &data[0]);
                fclose(f1);
                FILE *f2 = fopen("Sensor2.txt","r");
                fscanf(f2, "%d\n", &data[1]);
                fclose(f2); 
		        csv_file = fopen(filename, "w");
				fprintf(csv_file, " %d \n", data[0]+data[1]);
				fclose(csv_file); 	// Closing file because server will be closed by ctrl C,
	            buffer = "ok\n";
	            write(new_socket, buffer, strlen(buffer));
            }
        }
        else{
            // Closing the socket for client
            close(new_socket);
        }
    }
}
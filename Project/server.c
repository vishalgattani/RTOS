#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
	int server_socket, new_socket;
    struct sockaddr_in serv_add, client_add;
    int addrlen = sizeof(serv_add);
    char buff[1024];
    char *buffer;
    FILE *csv_file;
    char *filename = "sensor_recv.txt";  
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
            close(server_socket);

            // Message sending and receiving
            while(1){
                // Receing data from client
                printf("\nReceiving request message from client\n");
                read(new_socket, buff, sizeof(buff));
                printf("Request from client:%s\n",buff);

                csv_file = fopen(filename, "w");
    			fprintf(csv_file, " %s \n", buff);
    			fclose(csv_file); 	// Closing file because server will be closed by ctrl C,
                // Data to be sent back toclient
                //buffer = server_func(buff);

                // Sending the data nack to client
                //write(new_socket,buffer,sizeof(buffer));
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
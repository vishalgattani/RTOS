#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>
 #include <arpa/inet.h>
#define BUFSIZE 1024
#define SA struct sockaddr 
#define MAX_DATA_SIZE 50
#define PORT 8000


/* A simple routine calling UNIX write() in a loop */
static ssize_t loop_write(int fd, const void*data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;
        if ((r = write(fd, data, size)) < 0)
            return r;
        if (r == 0)
            break;
        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }
    return ret;
}

int main(int argc, char *argv[]) {
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
    
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;	  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server...\n"); 
  
    if(!fork()) {
        if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
            fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
            goto finish;
        }
        for (;;) {
            uint8_t buf[BUFSIZE];
            /* Record some data ... */
            if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
                fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
                goto finish;
            }
            /* And write it to STDOUT */
            if (loop_write(sockfd, buf, sizeof(buf)) != sizeof(buf)) {
                fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
                goto finish;
            }
        }
        ret = 0;
        finish:
        if (s)
            pa_simple_free(s);
        return ret;
        exit(1);
    }
    else {
        if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
            fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
            goto finish_play;
        }
        for (;;) {
            uint8_t buf[BUFSIZE];
            ssize_t r;
            
            // read
            if ((r = read(sockfd, buf, sizeof(buf))) <= 0) {
                if (r == 0) /* EOF */
                    break;
                fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
                goto finish_play;
            }
            // play
            if (pa_simple_write(s, buf, (size_t) r, &error) < 0) {
                fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
                goto finish_play;
            }
        }
        // play all samples
        if (pa_simple_drain(s, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
            goto finish_play;
        }
        ret = 0;
        finish_play:
            if (s)
                pa_simple_free(s);
        return ret;
    }
    return 0;
}

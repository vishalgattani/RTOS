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
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 1024
#define PORT 8000
#define SA struct sockaddr 


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
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    /* The Sample format to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    // to handle client requests
    for(;;) {
	    connfd = accept(sockfd, (SA*)&cli, &len); 
	    if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	    } 
	    else
		printf("server acccept the client...\n"); 
        // forking a child process to handle each request
        if(!fork()) {
            if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
            fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
            goto finish;
            }
            for (;;) {
                uint8_t buf[BUFSIZE];
                //read
                if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
                    fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
                    goto finish;
                }
                // send to stdout
                if (loop_write(connfd, buf, sizeof(buf)) != sizeof(buf)) {
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
            if ((r = read(connfd, buf, sizeof(buf))) <= 0) {
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
    }
return 0;
}

// C code to print 1 2 3 infinitely using pthread 
#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h> 
#include <sys/time.h>
  
// Declaration of thread condition variables 
pthread_cond_t cond1  =  
               PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond2  =  
               PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond3  =  
               PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond4  =  
               PTHREAD_COND_INITIALIZER; 
  
// mutex which we are going to 
// use avoid race condition. 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
   
// done is a global variable which decide  
// which waiting thread should be scheduled 
int done = 1; 
char *str = NULL;
int lower = 1;
int upper = 6;
int turn = 0;

int c1=0,c2=0,c3=0,c4=0;

// Thread function 
void *foo1(void *n) 
{ 
        while(1) { 
                turn = turn+1;
                // acquire a lock 
                pthread_mutex_lock(&lock); 
                   
                if (done != (int)*(int*)n) {           
                    pthread_cond_wait(&cond1, &lock);
                    //getchar(); 
                } 
                int num = (rand() % (upper - lower + 1)) + lower; 
                if(c1+num>100){
                    c1 = c1+0;
                }
                else if(c1+num<=100){
                    c1 = c1+num;
                }
                /* else if(c1==100){
                    pthread_cancel(pthread_self()); return NULL; 
                } */
                // done is equal to n, then print n 
                //printf("Turn number: %d\n",turn);
                printf("%d-%d ------------ %d\n", *(int*)n,num,c1); 
   
                // Now time to schedule next thread accordingly  
                // using pthread_cond_signal() 
                if (done == 1) { 
                        done = 2; 
                        pthread_cond_signal(&cond2); 
                } 
                // Finally release mutex 
                pthread_mutex_unlock(&lock); 
        } 
   
        //return NULL;  
} 

// Thread function 
void *foo2(void *n) 
{ 
        while(1) { 
                  
                // acquire a lock 
                pthread_mutex_lock(&lock); 
                   
                if (done != (int)*(int*)n) {           
                    pthread_cond_wait(&cond2, &lock);
                    //getchar(); 
                } 
                int num = (rand() % (upper - lower + 1)) + lower; 
                if(c2+num>100){
                    c2 = c2+0;
                }
                else if(c2+num<=100){
                    c2 = c2+num;
                }
                /* else if(c2==100){
                    return NULL;  
                } */
                // done is equal to n, then print n 
                printf("%d-%d ------------ %d\n", *(int*)n,num,c2); 
   
                // Now time to schedule next thread accordingly  
                // using pthread_cond_signal() 
                if (done == 2) { 
                        done = 3; 
                        pthread_cond_signal(&cond3); 
                } 
                // Finally release mutex 
                pthread_mutex_unlock(&lock); 
        } 
   
        //return NULL;  
} 

// Thread function 
void *foo3(void *n) 
{ 
        while(1) { 
                  
                // acquire a lock 
                pthread_mutex_lock(&lock); 
                   
                if (done != (int)*(int*)n) {           
                    pthread_cond_wait(&cond3, &lock);
                    //getchar(); 
                } 
                int num = (rand() % (upper - lower + 1)) + lower; 
                if(c3+num>100){
                    c3 = c3+0;
                }
                else if(c3+num<=100){
                    c3 = c3+num;
                }
                /* else if(c3==100){
                    return NULL;  
                } */
                // done is equal to n, then print n 
                printf("%d-%d ------------ %d\n", *(int*)n,num,c3); 
   
                // Now time to schedule next thread accordingly  
                // using pthread_cond_signal() 
                if (done == 3) { 
                        done = 4; 
                        pthread_cond_signal(&cond4); 
                } 
                // Finally release mutex 
                pthread_mutex_unlock(&lock); 
        } 
   
        //return NULL;  
} 

// Thread function 
void *foo4(void *n) 
{ 
        while(1) { 
                  
                // acquire a lock 
                pthread_mutex_lock(&lock); 
                   
                if (done != (int)*(int*)n) {           
                    pthread_cond_wait(&cond4, &lock);
                    //getchar(); 
                } 
                int num = (rand() % (upper - lower + 1)) + lower; 
                if(c4+num>100){
                    c4 = c4+0;
                }
                else if(c4+num<=100){
                    c4 = c4+num;
                }
                /* else if(c4==100){
                    return NULL;  
                } */
                // done is equal to n, then print n 
                printf("%d-%d ------------ %d\n\n", *(int*)n,num,c4); 
   
                // Now time to schedule next thread accordingly  
                // using pthread_cond_signal() 
                if (done == 4) { 
                        done = 1; 
                        pthread_cond_signal(&cond1); 
                } 
                // Finally release mutex 
                pthread_mutex_unlock(&lock); 
        } 
   
        //return NULL;  
} 

// Driver code 
int main() 
{ 
        struct timeval tvalBefore, tvalAfter;
        gettimeofday (&tvalBefore, NULL);
        time_t ltime; /* calendar time */
        ltime=time(NULL);
        printf("%.*s\n", (int)strlen(asctime(localtime(&ltime)))-1,asctime(localtime(&ltime)));
        
        srand(time(0));     
        pthread_t  tid1, tid2, tid3, tid4; 
        int n1 = 1, n2 = 2, n3 = 3, n4 = 4;
        
        // Create 3 threads  
        pthread_create(&tid1, NULL, foo1, (void *)&n1); 
        pthread_create(&tid2, NULL, foo2, (void *)&n2); 
        pthread_create(&tid3, NULL, foo3, (void *)&n3); 
        pthread_create(&tid4, NULL, foo4, (void *)&n4); 
        
        while(1){
            if(c1 == 100){
                gettimeofday (&tvalAfter, NULL);
                printf("Time in microseconds: %ld microseconds\n",((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L+tvalAfter.tv_usec) - tvalBefore.tv_usec);
                exit(0);
            }
            else if(c2 == 100){
                gettimeofday (&tvalAfter, NULL);
                printf("Time in microseconds: %ld microseconds\n",((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L+tvalAfter.tv_usec) - tvalBefore.tv_usec);
                exit(0);
            }
            else if(c3 == 100){
                gettimeofday (&tvalAfter, NULL);
                printf("Time in microseconds: %ld microseconds\n",((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L+tvalAfter.tv_usec) - tvalBefore.tv_usec);
                exit(0);
            }
            else if(c4 == 100){
                gettimeofday (&tvalAfter, NULL);
                printf("Time in microseconds: %ld microseconds\n",((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L+tvalAfter.tv_usec) - tvalBefore.tv_usec);
                exit(0);
            }

        }
        // infinite loop to avoid exit of a program/process 
        return 0;
} 
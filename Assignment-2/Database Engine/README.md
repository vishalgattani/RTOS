To run:
gcc server.c -o server
gcc client.c -o client 

Steps:
1) Run ./server to start the server.
2) Run ./client 0.0.0.0 <3rd argument> in a seperate terminal.

The third argument is necessary for this process as I was trying to use the filename as the third argument. The code can be changed to meet the needs by using two arguments also. However, that functionality is not implemented in this code for now.
The third argument can be anything. The code doesn't make use of it when we run the program.

It was used in calculating the values of RTT of server replies to generate 'readings.txt'.

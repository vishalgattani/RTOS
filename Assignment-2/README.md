### Echo Engine

1. We have a Process called Server (a separate executable) running on a computer. The folder where it has started has collection of text files (all ending with .txt extenstion). 
2. We have a five separate process (Called Clients) ClientA, ClientB, ClientC, ClientD, ClientE.  Each of them are running in separate folders.
3. Each client can communicate with Server through a TCP/IP.  Similarly server can send messages back to client through socket.
4. As the first step, a server and five clients that can communicate through socket. For example. The Client A sends an alphabet say.... 'd' to server, server receives it converts it into upper case and send it client. Like this every client will send an alphabet to server and it respond back with the corresponding upper case. In case if you send an upper case alphabet, the server responds with lower case. If you send any other character like *, $ % or numbers the server does not do anything it just respond back with the same character.

### Database Engine

Modified the functionality of both client and server.
1. As it was mentioned earlier the server has a collection of files in its folder.  Client send a request as string to the server the request will of the format 
2. GET <filename>  and server will respond to the request by sending the contents of the file back to the client as another string.
3. Eg  Client send GET marks.txt  then the server will respond back with contents of the file.
4. In case if the client send a filename which does not exist in server, the  server will respond with the string "Doesn't exist."

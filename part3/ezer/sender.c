#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/tcp.h>



#define PORT 9090
#define SIZE 1024

void sendToSock(int sock){
    FILE *fpr;
    char data[SIZE];
    char reno[6];//message received from client
    size_t bytes_read;
    size_t total = 0;
    for (int i = 0; i < 5 ; ++i) {
        printf("Iteration number %d\n", i+1);
        fpr = fopen("data.txt", "rb");
        if(fpr == NULL){
            printf("Error at opening file!\n");
            exit(1);
        }
        bzero(data, SIZE);
        while ((bytes_read = fread(data, 1, SIZE, fpr)) > 0){
            if(send(sock, data, SIZE, 0) < 0){
                printf("Error at sending message!\n");
                exit(1);
            }
            total += bytes_read;
            bzero(data, SIZE);
        }
    }
    printf("Total bytes sent: %zu\n",total);
    //server must be working while waiting to response from client
    while (1){
        bytes_read = recv(sock, reno, 6, 0);
        if(bytes_read < 0){
            printf("Error at receiving switch message!\n");
            exit(1);
        } else if(bytes_read == strlen("switch")){
            break;
        }
    }
}

int main() {
    char buff[256];
    //will determined congestion
    socklen_t len;
    //create a server socket
    int server_socket = socket(AF_INET, SOCK_STREAM,0);
    //define server's address
    struct  sockaddr_in server_address;
    
    
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    int flag = 1;
    //make sure that PORT is free from use
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1){
        perror("Port in use!\n");
        exit(1);
    }
    //bind the socket to our IP and PORT
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    //setting default congestion - cubic
    bzero(buff, 256);
    len = sizeof(buff);
    if(getsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0){
        printf("Error at getsockopt!\n");
        exit(1);
    }
    
    printf("Waiting for connection...\n");
    //wait for connections - 10 connection at most
    listen(server_socket, 10);
    //getting the client currently connecting
    int client_socket = accept(server_socket,NULL, NULL);
    printf("Connected to server!\n");
    printf("Current: %s\n", buff);
    //send a message to the client
    sendToSock(client_socket);
    //waiting for reno switch command from client
    printf("Received message! Switching congestion...\n");
    strcpy(buff, "reno");
    len = sizeof(buff);
    if(setsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, buff, len) != 0){
        printf("Error at setsockopt!\n");
        exit(1);
    }
    if(getsockopt(server_socket, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0){
        printf("Error at second getsockopt!\n");
        exit(1);
    }
    printf("Current: %s\n",buff);
    sendToSock(client_socket);
    close(server_socket);
    return 0;
}

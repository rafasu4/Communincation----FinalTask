#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define PORT 9090
#define SIZE 1024
#define FILE_SIZE 5242880


void receiveFromServer(int sock) {
    time_t start = time(NULL);
    time_t end;
    time_t diffTime;
    char data[SIZE];
    double avg;
    size_t total = 0;
    size_t bytes_read;
    bzero(data, SIZE);

    if((bytes_read = recv(sock, data, SIZE, 0)) < 0){
        exit(1);
    }
    total += bytes_read;
    while (total <FILE_SIZE) {
        bytes_read = recv(sock, data, SIZE, 0);
        total += bytes_read;
        bzero(data, SIZE);
    }
    end = time(NULL);
    printf("Total bytes received : %ld\n", total);
    diffTime = end - start;
    printf("Total time receive by seconds : %ld\n", diffTime);
    avg = (double) (diffTime) / 5;
    printf("AVG time of each iteration : %f\n", avg);
    //sending server to switch to reno
    char reno[] = "switch";
    int len = strlen(reno);
    if(send(sock, reno, len, 0) < 0){
        printf("Failed sending switch message!\n");
        exit(1);
    }
}

int main() {
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1) {
        printf("Could not create socket");
        exit(1);
    }
    //specify an address for the socket
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    
    //initializing server info
    server.sin_family = AF_INET;//type of address
    server.sin_port = htons(PORT);//converts a u_short from host to TCP/IP network byte order
    server.sin_addr.s_addr = htons(INADDR_ANY);//gets any address


    //connect to remote server
    if (connect(network_socket, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("Connection error!\n");
        exit(1);
    }
    printf("Connection established!\n");
    //received in cubic
    receiveFromServer(network_socket);
    printf("Switching to reno command has sent!\n");
    //received in reno
    receiveFromServer(network_socket);
    //close connection
    close(network_socket);


    return 0;
}

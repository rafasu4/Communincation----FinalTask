#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <vector>
#include "select.cpp"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/tcp.h>

#define SIZE 512

using namespace std;

struct prot_msg
{
    int MSG_ID, Source_ID, Destination_ID, Trailing_Msg, Function_ID;
    char payload[SIZE];

    prot_msg() {}
    prot_msg(int MSG_ID, int Source_ID, int Destination_ID, int Trailing_Msg, int Function_ID, string payload)
    {
        //build the packeta for now as default just insert them . but we will need to do HTONS and some .
        MSG_ID = MSG_ID;
        Source_ID = Source_ID;
        Destination_ID = Destination_ID;
        Trailing_Msg = Trailing_Msg;
        Function_ID = Function_ID;
        if (payload.size() > SIZE)
        {
            throw "Size of payload exceeding!";
        }
        payload = payload;
    }
};
struct node_data
{

    int id, ip, port;
    node_data(int id, int ip, int port) : id(id), ip(ip), port(port) {}
};

class node
{
    // Access specifier
public:
    // Data Members
    int def_sock = 0;
    int msg_id;
    map<int, prot_msg> sent;
    map<int, int> sib;

    int id;
    string ip;
    int port = 1234;

    // Member Functions()

    node() : id(-1), msg_id(0)
    {
    }

    void user_input(string s)
    {
        cout << "input from user:" << s << endl;
        std::vector<std::string> out;
        split_str(s, ',', out);
        string action = out[0];

        if (action == "connect")
        {
            string ad = out[1];
            cout << ad << endl;
            std::vector<std::string> address;
            split_str(ad, ':', address);
            if (this->id == -1)
                cout << "nack\n"
                     << endl;
            Connect(address[0], stoi(address[1]));
        }

        if (action = "setid")
        {

            //setid()
        }
    }

    void setid(int id)
    {
        this->id = id;
        int flag = 1;
        struct sockaddr_in server_socket;
        int ret, i;
        int r_port1 = 5000 + id;
        char buff[1025];
        time_t ticks;

        def_sock = socket(AF_INET, SOCK_STREAM, 0);

        memset(&server_socket, '0', sizeof(server_socket));

        server_socket.sin_family = AF_INET;
        server_socket.sin_addr.s_addr = htonl(INADDR_ANY);
        server_socket.sin_port = htons(r_port1);

        if (setsockopt(def_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1)
        {
            perror("Port in use!\n");
            exit(1);
        }
        bind(def_sock, (struct sockaddr *)&server_socket, sizeof(server_socket));
        //////////////////
        socklen_t len;

        bzero(buff, 1025);
        len = sizeof(buff);
        if (getsockopt(def_sock, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0)
        {
            printf("Error at getsockopt!\n");
            exit(1);
        }
        ////////////////

        printf("adding fd1(%d) to monitoring\n", def_sock);
        add_fd_to_monitoring(def_sock);
        listen(def_sock, 10);

        listen_to_fd();
    }

    //***********node is listeing**************************

    void listen_to_fd()
    {
        char buff[1025];
        int ret = 0;
        while (1)
        {
            printf("waiting for input...\n");
            ret = wait_for_input();
            //this file descriptor got message for you .
            printf("fd: %d is ready. reading...\n", ret);

            if (ret == 0)
            { //user command
                read(ret, buff, 1025);
                string s(buff);
                user_input(s);
            }
            if (ret == def_sock)
            {
                cout << " someone want to connect  me! i send him msg  " << endl;
                int client_socket = accept(def_sock, NULL, NULL);

                char data[SIZE];
                read(client_socket, buff, 1025);
                cout << buff << endl;
                int id = 0;

                sib[id] = client_socket;

                bzero(data, SIZE);

                char buff[SIZE] = "thank for connecting ,i'm 2 giving you ack";
                if (send(client_socket, buff, 512, 0) < 0)
                {
                    printf("Error at sending message!\n");
                    exit(1);
                }

                //add_fd_to_monitoring(network_socket);
            }
        }
    }

    int Connect(string ip_co, int port)
    {
        char *ip = &ip_co[0];

        int network_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (network_socket == -1)
        {
            printf("Could not create socket");
            exit(1);
        }
        struct sockaddr_in server_socket;
        bzero(&server_socket, sizeof(server_socket));
        //initializing  info
        server_socket.sin_family = AF_INET;   //type of address
        server_socket.sin_port = htons(port); //converts a u_short from host to TCP/IP network byte order
        if (inet_pton(AF_INET, ip, &server_socket.sin_addr) <= 0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        cout << "try to connect ip :" << ip << " port: " << port << endl;
        //connect mysocket->sock to  address .
        if (connect(network_socket, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        cout << "connect sucseed in socket number " << endl;

        //here i build mesagge and send it .
        char buff[1025];

        //here i need to build msg protocol
        char data[SIZE] = "hey i'm 1 and want to connect you";
        if (send(network_socket, data, 512, 0) < 0)
        {
            printf("Error at sending message!\n");
            exit(1);
        }

        bzero(data, SIZE);

        read(network_socket, data, 1025);
        cout << buff << endl;
        int id = 0;

        sib[id] = network_socket;
        //add_fd_to_monitoring(network_socket);

        //here i need to reacive msg and get the id of the node and add it.

        return 0;
    }
};

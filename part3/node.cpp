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
#include "select.h"
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
#include "prot_msg.cpp"
#define SIZE 512
using namespace std;

struct node_data
{

    int id, ip, port;
    node_data(int id, int ip, int port) : id(id), ip(ip), port(port) {}
};

class node
{
public:
    // Data Members
    int def_sock = 0;
    int msg_id;
    map<int, int> sib;
    map<int, prot_msg> sent;
    vector<prot_msg> message_sent;
    int id;
    string ip;

    /*Default constructor.*/
    node() : id(-1), msg_id(0) {}

    /*Receives input from user.*/
    void user_input(string s)
    {
        std::vector<std::string> out;
        split_str(s, ',', out);
        string action = out[0];

        if (action == "connect")
        {
            string ad = out[1];
            std::vector<std::string> address;
            split_str(ad, ':', address);
            if (this->id == -1)
                cout << "nack\n"
                     << endl;
            Connect(address[0], stoi(address[1]));
        }

        if (action == "setid")
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
        add_fd_to_monitoring(def_sock);
        printf("FD(%d) successfully added to monitor!\n", def_sock);
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
            printf("Waiting for connection...\n");
            ret = wait_for_input();
            //this file descriptor got message for you .
            printf("FD:%d is in use! Reading...\n", ret);
            //user command
            if (ret == 0)
            {
                read(ret, buff, 1025);
                string s(buff);
                user_input(s);
            }
            //from other to me .
            else
            {
                //if connection come to me i need to accept it first :
                int client_socket = accept(def_sock, NULL, NULL);
                char buffer[SIZE];
                //add_fd_to_monitoring(client_socket);
                read(client_socket, buffer, SIZE);
                prot_msg message(buffer);
                string send_back;
                switch (message.func_id)
                {
                //connect
                case 4:
                {
                    addZero(send_back, this->msg_id++);
                    send_back += to_string(this->msg_id);
                    addZero(send_back, this->id);
                    send_back += to_string(this->id);
                    addZero(send_back, message.src_id);
                    send_back += to_string(message.src_id);
                    send_back += "00000001";//trail + fun_id 
                    send_back += to_string(message.msg_id);
                    break;
                }
                default:
                    break;
                }
                char c[send_back.length()];
                strcpy(c, send_back.c_str());
                int id = message.src_id;
                sib[id] = client_socket;
                if (send(client_socket, c, 512, 0) < 0)
                {
                    printf("nack\n");
                }
                add_fd_to_monitoring(client_socket);
            }
        }
    }

    void Connect(string ip_co, int port)
    {
        //string to char*
        char *ip = &ip_co[0];
        //if this node doesn't have an id
        if (this->id == -1)
            cout << "nack\n"
                 << endl;
        int network_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (network_socket == -1)
        {
            cout << "nack\n"
                 << endl;
        }
        struct sockaddr_in server_socket;
        bzero(&server_socket, sizeof(server_socket));
        //initializing  info
        server_socket.sin_family = AF_INET;   //type of address
        server_socket.sin_port = htons(port); //converts a u_short from host to TCP/IP network byte order
        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, ip, &server_socket.sin_addr) <= 0)
        {
            cout << "nack\n"
                 << endl;
        }

        cout << "My socket is:" << network_socket << endl;
        cout << "Trying to connect IP :" << ip << " Port: " << port << endl;
        //connect mysocket->sock to  address .
        if (connect(network_socket, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0)
        {
            cout << "nack\n"
                 << endl;
        }
        cout << "Connection established! " << endl;
        prot_msg data(this->msg_id++, this->id, 0, 0, 4, "Hello");
        string s = data.msgToStr();
        char c[s.length()];
        strcpy(c, s.c_str());
        if (send(network_socket, c, 512, 0) < 0)
        {
            cout << "nack\n"
                 << endl;
        }
        char buff[SIZE];
        bzero(buff, SIZE);
        //accept ack
        read(network_socket, buff, SIZE);
        prot_msg ack(buff);
        if(ack.payload != to_string(this->msg_id - 1)){
            cout<<"nack"<<endl;
        }
        else{
            cout<<"ack"<<endl;
            int id = ack.src_id;
            sib[id] = network_socket;
            add_fd_to_monitoring(network_socket);
        }
    }
};

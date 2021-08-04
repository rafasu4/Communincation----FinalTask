#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include "select.h"
#include <arpa/inet.h>
#include <time.h>
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
    int id;
    string ip;

    /*Default constructor.*/
    node() : id(-1), msg_id(0) {}

    /*Receives input from user.*/
    void user_input(string s)
    {
        vector<string> out;
        split_str(s, ',', out);
        string action = out[0];
        // for(string s:out)cout<<s<<endl;
        // cout<<out.size();

        if (action == "connect")
        {
            string ad = out[1];
            vector<string> address;
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

        if (action=="send"){
            string r = out[3].substr(0, stoi(out[2]));
          
            prot_msg data(msg_id++, id, stoi(out[1]), 0, 32, r);
            mysend(data);
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
           
            printf("FD:%d is in use! Reading...\n", ret);
            //user command
            if (ret == 0)
            {
                read(ret, buff, 1025);
                string s(buff);
                user_input(s);
            }
            //only connect .
            if (ret==def_sock)
            {
                
                int client_socket = accept(def_sock, NULL, NULL);
                //read
                char buffer[SIZE];
                read(client_socket, buffer, SIZE);
                prot_msg message(buffer);
                message.print();
               
                //build msg_back
                
                bzero(buff, SIZE);
                
                prot_msg msg(this->msg_id++,this->id,message.src_id,0,1,to_string(message.msg_id));
                string send_back=msg.msgToStr();
                
                strcpy(buff, send_back.c_str());
                

              
                //send
                if (send(client_socket, buff, SIZE, 0) < 0)
                {
                    printf("nack\n");
                }
                // sent[msg.msg_id]=msg;
                
                //update 
                sib[message.src_id] = client_socket;
                add_fd_to_monitoring(client_socket);
            }
            if(ret>3){
                char buffer[SIZE];
                read(ret, buffer, SIZE);
                prot_msg message(buffer);
                message.print();

                
            }
            
            
        }

       
    }




































    void Connect(string ip_co, int port)
    {
        //string to char*
        char *ip = &ip_co[0];
        //if this node doesn't have an id
        if (this->id == -1){
            cout << "nack\n"<< endl;}

        int network_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (network_socket == -1)
        {
            cout << "nack\n"<< endl;
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
        //connect mysocket->sock to  addres

        if (connect(network_socket, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0)
        {
            cout << "nack\n"
                 << endl;
        }
        cout << "Connection established! " << endl;

        //build msg
        prot_msg data(this->msg_id++, this->id, 0, 0, 4, "");
        string s = data.msgToStr();
        char buff[SIZE];
        strcpy(buff, s.c_str());

        //send
        if (send(network_socket, buff, SIZE, 0) < 0)
        {
            cout << "nack\n"
                 << endl;
        }
        // sent[data.msg_id]=data;
        
        
        //read
        bzero(buff, SIZE);
        read(network_socket, buff, SIZE);
        prot_msg ack(buff);
        ack.print();
        //check
        if(stoi(ack.payload) != data.msg_id){
            
            cout<<"nack"<<endl;
        }
        else{
            cout<<"ack"<<endl;
            //update
            int id = ack.src_id;
            sib[id] = network_socket;
           add_fd_to_monitoring(network_socket);
        }
    }












     void mysend(prot_msg p){
            string s = p.msgToStr();
            char buff[SIZE];
            strcpy(buff, s.c_str());
            
            if (send(sib[p.dest_id], buff, SIZE, 0) < 0)
        {
            cout << "nack\n"
                 << endl;
        }
        
         


        }
};

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "select.h"

#define SIZE 512
#define IP "192.168.1.135"

using namespace std;

struct prot_msg
{
    int MSG_ID, Source_ID, Destination_ID, Trailing_Msg, Function_ID;
    char payload[SIZE];

    prot_msg(){}
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
    int msg_id;
    vector<node_data> sibs;
    map<int, prot_msg> sent;
    vector<prot_msg> message_sent;
    int id;
    string ip;
    int port = 1234;

    // Member Functions()

    node() : id(-1), msg_id(0)
    {
    }

    void setid(int id)
    {
        this->id = id;
        int listenfd = 0, listenfd2=0;
    struct sockaddr_in serv_addr; 
    int ret, i;
    int r_port1=5000+id, r_port2=6000 + id;
    char buff[1025];
    time_t ticks; 
    
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    listenfd2 = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(r_port1); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(r_port2);  
    bind(listenfd2, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    
    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);
    printf("adding fd2(%d) to monitoring\n", listenfd2);
    add_fd_to_monitoring(listenfd2);
    listen(listenfd, 10);
    listen(listenfd2, 10); 

for (i=0; i<10; ++i)
	  {
	    printf("waiting for input...\n");
	    ret = wait_for_input();
	    printf("fd: %d is ready. reading...\n", ret);
	    read(ret, buff, 1025);
	    printf("\"%s\"", buff);
	}
    }

    void recieve_ack(prot_msg p, int ip, int port)
    {
        int id_msg_reply = stoi(((string)p.payload).substr(0, 4));
        //if the meassge that the ack answere' was sent.
        if (sent.find(id_msg_reply) != sent.end()){
            cout << "nack" << endl;
        }
        else
        {
            prot_msg sent_p = sent[id_msg_reply];
            switch (sent_p.Function_ID){

            case 4:
             //if its answer to my connect msg add him!
                node_data node(p.Source_ID, ip, port);
                sibs.push_back(node);

                break;
            }
        }
    }

    //how i get the ip and port from the TCP ?
    void recieve(prot_msg p, int ip, int port)
    {

        switch (p.Function_ID){
        case 1: //ack
            cout << "ack";
            recieve_ack(p, ip, port);

            break;
        case 2: //nack
            cout << "nack";
            break;
        case 4:
        
            //connect
            string s = to_string(p.MSG_ID);
            prot_msg pack(this->msg_id, this->id, p.Source_ID, 0, 1, s);
            node_data node(p.Source_ID, ip, port);
            sibs.push_back(node);
            sent[p.MSG_ID] = pack;
            send_packet_TCP(pack, node);

            break;
        }
    }

    void Connect(int ip, int port)
    {
        //if this node doesn't have an id
        if (this->id == -1)
        {
            cout << "nack\n" << endl;
        }
        else
        {
            //open TCP socket 
            int network_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (network_socket == -1)
            {
                cout << "nack\n";
            }
            else
            {
                add_fd_to_monitoring(network_socket); 
                //specify an address for the socket
                struct sockaddr_in server;
                bzero(&server, sizeof(server));
                //initializing server info
                server.sin_family = AF_INET;        //type of address
                server.sin_port = htons(port);      //converts a u_short from host to TCP/IP network byte order
                server.sin_addr.s_addr = htons(ip); 
                //connect to remote node
                if (connect(network_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
                {
                    cout<<"nack\n";
                }
                cout<<"Connection established!\n";             
                // build and save msg
                prot_msg pack(msg_id++, this->id, 0, 0, 4, "");
                send(network_socket, (void*)&pack, SIZE, 0);
                sent[pack.MSG_ID] = pack;
                //prot_msg answer;
                char * data = "Hi from node1!\n";
                if(recv(network_socket, data , sizeof(data), 0) < 0){
                    cout << "nack\n";
                }
                // if(answer.Destination_ID != this->id || answer.Source_ID != pack.Destination_ID || answer.Function_ID != 4 ){
                //     cout<<"nack\n";
                // }
                else{
                    // //send him the packet.
                    node_data neighbor(id, ip, port);
                    this->sibs.push_back(neighbor);
                    //cout<<"New node added to neighbors: " << answer.Source_ID<<"\n";
                }

            }
        }
    }

    void send_packet_TCP(prot_msg p, node_data d)
    {
        // need to send in TCP .
    }
};
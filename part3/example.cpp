#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include "select.h"
int main(int argc, char *argv[])
{
    int listenfd = 0, listenfd2=0;
    struct sockaddr_in serv_addr; 
    int ret, i;
    int r_port1=5000,r_port2=5001;
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



//************************* my fuckk


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
            

            break;
        }
    }
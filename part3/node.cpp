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

#define MAX 1000
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
    map<int, string> sent;
    map<int,int> count_discovers;
    map<int,string>temp_route;
    map<int,int> temp_len;
    int id;
    string ip;
    int color=0;
    vector<int> my_disco;

    /*Default constructor.*/
    node() : id(-1), msg_id(0) {}

    /*Receives input from user.*/
    /*Receives input from user.*/
     void mysend(prot_msg p){
        mysend(p,sib[p.dest_id]);}

    int mysend(prot_msg p,int fd){
            if(fd<3){
                cout<<"bad fd in send"<<endl;
                
            }

            string s = p.msgToStr();
            char buff[SIZE];
            strcpy(buff, s.c_str()); 
            if (send(fd, buff, SIZE, 0) < 0)
        {
            cout << "nack\n"<< endl;
            return-1;
        }
        cout<<"message sent! "<<endl;
        
        sent[p.msg_id]=p.msgToStr();
        p.print();
        // prot_msg a(sent[p.msg_id].c_str);
        // string v(sent[p.msg_id]);
        // char buf[SIZE];
        // strcpy(buf, v.c_str());
        // prot_msg a (buf);
        // a.print();

        
        return 1;
        }

    prot_msg myread(int fd){
        char buffer[SIZE];
        int a = read(fd, buffer, SIZE);
        if (a==-1)cout<<"nack"<<endl;
        if (a==0){

            //need to delete socket and tell all the network .
            //need to searche in the map the key of this fd 
            return prot_msg(-1,-1,-1,-1,-1,"");
        }
        prot_msg message(buffer);
        cout<<"message recived!"<<endl;
       message.print();
        return message;

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
        char buff[SIZE];
        //build msg
        prot_msg data(this->msg_id++, this->id, 0, 0, 4, "");
        mysend(data,network_socket);
        
        //read
        
        prot_msg ack=myread(network_socket);
        
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






























 void listen_to_fd()
    {
        char buff[1025];
        int ret = 0;


        while (1)
        {
            printf("Waiting...\n\n\n");
            ret = wait_for_input();
            printf("FD:%d is in use! Reading...\n", ret);
            
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
                prot_msg message=myread(client_socket);
                
               
                //build msg_back
                prot_msg msg(this->msg_id++,this->id,message.src_id,0,1,to_string(message.msg_id));
                mysend(msg,client_socket);
                
                //update 
                sib[message.src_id] = client_socket;
                add_fd_to_monitoring(client_socket);
            }
            if(ret>3){

                prot_msg message=myread(ret);
                
                handle(message);
               
                
            }
            
            
        }

       
    }
   

   
    


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
            string len=addZero(stoi(out[2]));
            cout<<stoi(out[2])<< len <<endl;
            string r = out[3].substr(0, stoi(out[2]));
            prot_msg msg(msg_id++, id, stoi(out[1]), 0, 32, len+r);

            if ( sib.find(msg.dest_id) == sib.end() )
                {
                    cout<<"searchig a way ..."<<endl;
                    send_dicovers(msg.dest_id,"my");
                } 
                else 
                {
             
                     mysend(msg);
                }
            
            
        }


    }

    void send_with_relly(){}
    int handle_route(prot_msg p){

string s =p.payload.substr(0,4);





 int id_origin=remove_zero_stoi(s);

        string v(sent[id_origin]);
        char buf[SIZE];
        strcpy(buf, v.c_str());
        prot_msg a (buf);
        cout<<"this is the origin discover"<<endl;
        a.print();
        int final_dest=stoi(a.payload);

        string path = p.payload.substr(9);
        int path_len=stoi(p.payload.substr(4,8));
        
        
        if  ((count(my_disco.begin(), my_disco.end(), id_origin)))
{
            cout<<"i found the way to :"<<final_dest<<endl;
            
            return 2;
}

        // //i need to check if it the best way.
        // //in discover i need to put max value in path len ,
        // if(path_len<temp_len[final_dest]&&path_len!=0){
        //     temp_len[final_dest]=path_len;
        //     temp_route[final_dest]=path;
        // }
        // count_discovers[final_dest]=count_discovers[final_dest]-1;
        // if(count_discovers[final_dest]==0)
        // {
        // //it means i got answer from all my discovers and i can return in route the best way i got .
        // string path_to_route=path+to_string(id);
        // prot_msg route_back( msg_id++,id,  discober_original_id.src_id,0,32,path_to_route);
        // mysend(route_back);
        // }




return 0;
    }
    int handle (prot_msg p){

        if(p.func_id==0){
        cout<<"emptybuffer" <<endl;
        return 0; 
        }
        if(p.func_id==1){
            //ack
        }
        if(p.func_id==2){
            //nack
        }
        if(p.func_id==8){
            
             handle_discover(p);
             }
             
        

        if (p.func_id==16){

            handle_route(p);
        }

        if (p.func_id==32){
        
            prot_msg msg(msg_id++,id,p.src_id,0,1,to_string(p.msg_id));
            mysend(msg);
            
        }
    return 0;

    }
    ///////////////////////////////HANDLE DISCOVER///////////////
     int handle_discover(prot_msg p){
         // if i'm grey i will no send discovers its use less i return route with 0.
         //how to avoid circle?
        cout<<"i got handle discover"<<endl;
        if (color==1){
            return -1 ;}
        //if color=0 you open to discover.
        cout<<p.payload<<endl;
        int final_dest= stoi(p.payload);
        cout<<"this is the final_dest i asked you to look"<<final_dest<<endl;
        
                if ( sib.find(final_dest) == sib.end() )
                {
                    
                    cout<<"i'l go search..."<<endl;
                    send_dicovers(final_dest,"");

                } 
                else 
                {
                    cout<<"found a way!!"<<endl;
                    //i find a way! lets tell him
                    string s=addZero(p.msg_id)+addZero(2)+addZero(id)+addZero(final_dest);
                    prot_msg ack_route(msg_id++,id,p.src_id,0,16,s);
                    mysend(ack_route);
                }
        return 0;


    }

    void send_dicovers(int dest,string flag){

            count_discovers[dest]=0;
            temp_len[dest]=MAX;
            //dont look for wat throght me. 
            
            for (auto const& x : sib)
            {
                  cout<<"i got sibs to search ..."<<endl;
            prot_msg msg(msg_id++,id,x.first,0,8,to_string(dest));
            if (mysend(msg,x.second)!=-1){
                count_discovers[dest]=count_discovers[dest]+1;
                            }

            //i need to know my when i started dicovers
            if(flag=="my"){
            my_disco.push_back(msg.msg_id);
            }
            
            // cout<<"number of discover sent : "<<count_discovers<<endl;
            color=1;
    }
    }
    //////////////////////////////HANDLE ROUTE///////////////


  



    
    
   

























    

   

    
};

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#define SIZE 492

using namespace std;


struct prot_msg{
int MSG_ID,Source_ID,Destination_ID,Trailing_Msg,Function_ID;
string payload;


prot_msg(int MSG_ID,int Source_ID,int Destination_ID,int Trailing_Msg,int Function_ID,string payload )
{
//build the packeta for now as default just insert them . but we will need to do HTONS and some .
 MSG_ID=MSG_ID; Source_ID=Source_ID;Destination_ID=Destination_ID;Trailing_Msg=Trailing_Msg;Function_ID=Function_ID;payload=payload;

}

};
struct node_data{
int id,ip,port;
node_data(int id ,int ip,int port):id(id),ip(ip),port(port){}};



class node
{
    // Access specifier
    public:
 
    // Data Members
    int msg_id;
    vector<node_data> sibs;
    map<int, prot_msg> sent;
    vector<prot_msg> message_sent;
    int id ;
    string ip;
    int port =1234;
 
    // Member Functions()
    
    node():id(-1),msg_id(0){
    
    }


    void setid(int id)
    {
       this->id=id;
    }


    void recieve_ack(prot_msg p,int ip,int port){

    int id_msg_reply = stoi(p.payload.substr(0, 4));
    //if the meassge that the ack answere' was sent.
    if (sent.find(id_msg_reply) != sent.end())
         cout<<"nack"<<endl;
         
    else 
    {

    prot_msg sent_p=sent[id_msg_reply];
    switch (sent_p.Function_ID) {
       
        case 4:{//if its answer to my connect msg add him!
            node_data node(p.Source_ID,ip,port);
            sibs.push_back(node);

            break;
            }
        default:
    }



       
    
    }

    }
    //how i get the ip and port from the TCP ? 
    void recieve(prot_msg p,int ip,int port)
    {

        switch (p.Function_ID) {
        case 1://ack
            cout <<"ack";
            recieve_ack(p,ip,port);
        
            break;
        case 2://nack
            cout <<"nack";
            break;
        case 4:{
            //connect
            string s = to_string(p.MSG_ID);
            prot_msg pack(this->msg_id,this->id,p.Source_ID,0,1,s);
            node_data node(p.Source_ID,ip,port);
            sibs.push_back(node);
            sent[p.MSG_ID]=pack;
            send_packet_TCP(pack,node); 

            break;}
        default:
    }
    }
    int open_tcp(int ip,int port){


    } 

    void connect(int ip,int port){
            if(this->id==-1)cout<<"you need to set id first">>endl;
            else{
            //open TCP socket to IP and Port .
            //if failed print "nack"
            if(open_tcp( ip, port)==-1){
                cout<<"nack"<<endl;
            }

            //send him the packet.
            node_data d(id,ip,port);
            
        
            // build and save msg
            prot_msg pack(msg_id++,id,0,0,4,"");
            
            sent[pack.MSG_ID]=pack;
            send_packet_TCP(pack,d);  
    }}
    
    void send_packet_TCP(prot_msg p,node_data d){
        // need to send in TCP .

    }
    
    


    


};
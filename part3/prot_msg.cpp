#include "iostream"
#include "select.h"


#define SIZE 512

using namespace std;

struct prot_msg
{
  
    int msg_id, src_id, dest_id, trail, func_id;
    string payload;

    prot_msg(const char *buff)
    {
        string msg_id_c, src_id_c, dest_id_c, trail_c, func_id_c;
        string payload_s;

        for (int i = 0; i < 4; i++)
        {
            msg_id_c += buff[i];
            src_id_c += buff[4 + i];
            dest_id_c += buff[8 + i];
            trail_c += buff[12 + i];
            func_id_c += buff[16 + i];
        }
        msg_id = stoi(msg_id_c);
        src_id = stoi(src_id_c);
        dest_id = stoi(dest_id_c);
        trail = stoi(trail_c);
        func_id = stoi(func_id_c);
        payload = ((string)buff).substr(20);
        
    }

    prot_msg(int msg, int src, int dest, int tr, int func, string pl)
    {
        //build the packeta for now as default just insert them . but we will need to do HTONS and some .
        msg_id = msg;
        src_id = src;
        dest_id = dest;
        trail = tr;
        func_id = func;
        if (pl.size() > SIZE - 20)
        {
            throw "Size of payload exceeding!";
        }
        payload = pl;
    }

    string msgToStr()
    {
        string s; 
        addZero(s, this->msg_id);
        s += to_string(msg_id);
        addZero(s, this->src_id);
        s += to_string(this->src_id);
        addZero(s, this->dest_id);
        s += to_string(this->dest_id);
        addZero(s, this->trail);
        s += to_string(this->trail);
        addZero(s, this->func_id);
        s += to_string(this->func_id);
        s += payload;
        return s;
    }
    int get_id(){
    return msg_id;}
     void print(){
         
      cout<<"message  id  :"<<msg_id<<endl;
      cout<<"source   id  :"<<src_id<<endl;
      cout<<"dest     id  :"<<dest_id<<endl;
      cout<<"trail    msg :"<<trail<<endl;
      cout<<"function id  :"<<func_id<<endl;
      cout<<"pay load     :"<<payload<<endl;
    }


  
   
};
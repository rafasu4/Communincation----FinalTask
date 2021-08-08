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

class node
{
public:
    /*Fields*/
    /*Listener socket.*/
    int def_sock = 0;
    /*Messages counter.*/
    int msg_id;
    /*Holds the id of neighbors nodes as key, and their socket FD as value.*/
    map<int, int> sib;
    /*History of all sent messages.*/
    map<int, string> sent;
    /*Number of times this node send discover. key - destination node, value - number of discover to this node.*/
    map<int, int> count_discovers;
    /*Collects all path received after rout.*/
    map<int, string> temp_route;
    /*Holds the destination node as a key and the length of the path to it as a value.*/
    map<int, int> temp_len;
    map<int, vector<string>> received;
    /*Represents a path from one node to another. Key - the destination, Value - path builds from nodes to dest.*/
    map<int, vector<int>> path;
    /*Map that holds messages waiting for a path. Key - discover msg, Value - msg to be sent.*/
    vector<prot_msg> wait_to_send;
    /*This node id.*/
    int id;
    /*Use to check if i started the discover.*/
    vector<int> my_disco;
    /*Holds all the relayed messages this node received.*/
    vector<prot_msg> relayed;
    vector<string> broadcast;
    /*Holds info to whom we need send a an ack after relaying - where Key - massage id, Value - source node.*/
    vector<pair<int, int>> ack_to_relay;

    /*Default constructor.*/
    node() : id(-1), msg_id(0) {}

    /*Sends message.*/
    void mysend(prot_msg p)
    {
        cout << p.dest_id << endl;
        mysend(p, sib[p.dest_id]);
    }
    /*Sends message to specific FD.*/
    int mysend(prot_msg p, int fd)
    {
        string s = p.msgToStr();
        char buff[SIZE];
        strcpy(buff, s.c_str());
        if (send(fd, buff, SIZE, 0) < 0)
        {
            cout << "nack\n"
                 << endl;
            return -1;
        }
        cout << "message sent!" << endl;
        sent[p.msg_id] = p.msgToStr();
        return 1;
    }

    /*Read from given FD.*/
    prot_msg myread(int fd)
    {
        char buffer[SIZE];
        int a = read(fd, buffer, SIZE);
        if (a == -1)
            cout << "nack" << endl;
        if (a == 0)
        {
            //remove from monitor
            remove_fd(fd);
            int remove_id = dele_map(fd, sib);
            cout << "node has been termianted and deleted : " << remove_id << endl;
            //remove from sibs
            sib.erase(remove_id);
            //send broadcast
            string m = "0001" + addZero(remove_id);
            prot_msg broad(msg_id++, id, 888, 0, 0, m);
            // send_broadcast(broad);
            //need to delete socket and tell all the network .
            //need to searche in the map the key of this fd
            return prot_msg(-1, -1, -1, -1, -1, "");
        }
        prot_msg message(buffer);
        cout << "message received!" << endl;
        received[message.msg_id].push_back(message.msgToStr());
        message.print();
        return message;
    }
    /*Set this node id.*/
    void setid(int id)
    {
        this->id = id;
        int flag = 1;
        struct sockaddr_in server_socket;
        int r_port1 = 5000 + id;
        char buff[SIZE];

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

        bzero(buff, SIZE);
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
        if (this->id == -1)
        {
            cout << "nack\n"
                 << endl;
        }

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


        if (connect(network_socket, (struct sockaddr *)&server_socket, sizeof(server_socket)) < 0)
        {
            cout << "nack\n"
                 << endl;
        }
        cout << "Connection established! " << endl;
        //build msg
        prot_msg data(this->msg_id++, this->id, 0, 0, 4, "");
        mysend(data, network_socket);

        //read

        prot_msg ack = myread(network_socket);

        //check
        if (stoi(ack.payload) != data.msg_id)
        {

            cout << "nack" << endl;
        }
        else
        {
            cout << "ack" << endl;
            //update
            int id = ack.src_id;
            sib[id] = network_socket;
            add_fd_to_monitoring(network_socket);
        }
    }

    void listen_to_fd()
    {
        char buff[SIZE];
        int ret = 0;

        while (1)
        {
            printf("Waiting...\n\n\n");
            ret = wait_for_input();
            printf("FD:%d is in use! Reading...\n", ret);

            if (ret == 0)
            {
                bzero(buff, SIZE);
                read(ret, buff, SIZE);
                string s(buff);
                user_input(s);
            }

            //only connect .
            if (ret == def_sock)
            {
                int client_socket = accept(def_sock, NULL, NULL);
                //read
                prot_msg message = myread(client_socket);

                //build msg_back
                prot_msg msg(this->msg_id++, this->id, message.src_id, 0, 1, to_string(message.msg_id));
                mysend(msg, client_socket);

                //update
                sib[message.src_id] = client_socket;
                add_fd_to_monitoring(client_socket);
            }
            if (ret > 3)
            {

                prot_msg message = myread(ret);
                handle(message);
            }
        }
    }

    void user_input(string s)
    {
        vector<string> out;
        split_str(s, ',', out);
        if (out.size() == 1)
        {
            split_str(s,' ', out);
            if(out[0] == "peers"){
                peers(sib);
            }
            else{
                cout <<"Invalid input"<<endl;
            }
        }
        else
        {
            string action = out[0];
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

            else if (action == "setid")
            {
                if (stoi(out[1]) > 0)
                {
                    this->setid(stoi(out[1]));
                }
                else
                {
                    cout << "Invalid input" << endl;
                    cin >> s;
                    user_input(s);
                }
            }

            else if (action == "send")
            {
                string len = addZero(stoi(out[2]));
                string r = out[3].substr(0, stoi(out[2]));
                prot_msg msg(msg_id++, id, stoi(out[1]), 0, 32, len + r);
                //if node doesn't have destination as sibling - discover
                if (sib.find(msg.dest_id) == sib.end())
                {
                    wait_to_send.push_back(msg);
                    send_dicovers(msg.dest_id, "my", -1);
                }
                else
                {
                    mysend(msg);
                }
            }

            else if (action == "route")
            {
                send_dicovers(stoi(out[1]), "my", -1);
            }
            else
            {
                cout << "Invalid input" << endl;
                cin >> s;
                user_input(s);
            }
        }
    }

    void relay(prot_msg relay_msg)
    {
        bool flag = flag;
        int origin = relay_msg.src_id; //the node that started the relay chain
        vector<prot_msg>::iterator it = relayed.begin();
        //go over all the saved relay msgs received
        while (it != relayed.end())
        {
            //if the it has the same source and the msg's ids are Consecutive - there is a match
            if (it->src_id == origin && it->msg_id == relay_msg.msg_id - 1)
            {
                string payload = it->payload;                  //who sand me this relay massage
                payload = payload.substr(4);                   //removing the src of relay to update path
                string next_in_line = payload.substr(4, 4);    //getting the last part of the payload - the next node to relay
                prot_msg relay_to_next_in_line(it->msg_id, origin, stoi(next_in_line), it->trail, 64, payload);
                cout << "Relay massage to: " << relay_to_next_in_line.dest_id << endl;
                mysend(relay_to_next_in_line);
                // ack_to_relay.push_back({relay_to_next_in_line.msg_id, src_of_relay});
                //if the received msg has more than one fragment
                if (it->trail > 1)
                {
                    it->trail--;
                }
                //the last fragment of msg is received
                else
                {
                    it = relayed.erase(it); //remove from need to relayed msg
                }
                mysend(relay_msg, sib[stoi(next_in_line)]);
                it = relayed.end();
            }
            else
            {
                it++;
            }
        }
    }

    int handle_route_func(prot_msg discover_pack, prot_msg route_pack, int flag_rly, int final_dest)
    {
        string path = route_pack.payload.substr(8);
        int path_len = stoi(route_pack.payload.substr(4, 4));
        if (path_len < temp_len[final_dest] && path_len != 0)
        {

            temp_len[final_dest] = path_len;
            temp_route[final_dest] = path;
        }
        count_discovers[final_dest] = count_discovers[final_dest] - 1;

        if (count_discovers[final_dest] == 0)
        {
            //if i"m the root go relay the path!
            if (flag_rly)
            {
                cout << path << endl;
                //if the returned path is zero - no path has been found
                if (stoi(path) == 0)
                {
                    cout << "nack" << endl;
                }
                else
                {
                    bool flag = false;
                    for (auto &el : wait_to_send)
                    {
                        if (el.dest_id == final_dest)
                        {
                            cout << "Relaying and messaging... " << endl;
                            prot_msg msg = el;
                            path = addZero(id) + path;
                            prot_msg relay(msg_id++, id, stoi(path.substr(4, 4)), msg.trail, 64, path);
                            mysend(relay);
                            msg.msg_id = msg_id++;
                            mysend(msg, sib[stoi(path.substr(4, 4))]);
                            flag = true;
                            break;
                        }
                    }
                    if (!flag)
                    {
                        int len = path.length() / 4;
                        int i = 0;
                        for (; i < len - 4; i += 4)
                        {
                            cout << stoi(path.substr(i, i + 4)) << "->";
                        }
                        cout << stoi(path.substr(i, i + 4));
                    }
                }
            }
            //if i"m not the root pass it
            else
            {
                //it means i got answer from all my discovers and i can return in route the best way i got .
                string path_to_route = addZero(id) + temp_route[final_dest];
                string len = addZero(temp_len[final_dest] + 1);
                string m = addZero(discover_pack.msg_id) + len + path_to_route;
                prot_msg route_back(msg_id++, id, discover_pack.src_id, 0, 16, m);
                mysend(route_back);
            }
        }
        return 0;
    }

    int handle_route(prot_msg p)
    {
        //get id of discover .
        int id_origin = remove_zero_stoi(p.payload.substr(0, 4));
        //get final dest
        string final_d = p.payload.substr(p.payload.length() - 4, 4);
        int final_dest = (stoi(final_d));
        int my_search = (count(my_disco.begin(), my_disco.end(), id_origin));
        //if its my route send it .
        if (my_search)
            handle_route_func(sent[id_origin], p, 1, final_dest);

        for (string msg : received[id_origin])
        {
            prot_msg a(msg);
            //i check if this is a discover to my final dest. so if found what i wanted.
            if ((a.func_id == 8) && remove_zero_stoi(a.payload) == final_dest)
            {
                // temp_d.push_back(msg);
                handle_route_func(a, p, 0, final_dest);
            }
        }
        return 0;
    }

    /*Handles hhe received massage by its function id.*/
    int handle(prot_msg p)
    {
        if (p.dest_id == 888)
        {
            string hash = addZero(p.msg_id) + addZero(p.src_id);
            //if you didnt receive this broadcast
            if (std::find(broadcast.begin(), broadcast.end(), hash) == broadcast.end())
            {
                broadcast.push_back(hash);
                int to_delete = stoi(p.payload.substr(4, 4));
                //if it your sib delete him
                if (sib.count(to_delete) > 0)
                {
                    //remove from monitor
                    remove_fd(sib[to_delete]);
                    cout << "node has been termianted and deleted : " << to_delete << endl;
                    //remove from sibs
                    sib.erase(to_delete);
                    //send broadcast

                    send_broadcast(p);
                }
                else
                {
                    send_broadcast(p);
                }
            }
            else
            {
                cout << "Already got that broadcast" << endl;
            }
        }
        //ack received
        if (p.func_id == 1)
        {
            for (auto &relayer : ack_to_relay)
            {
                if (relayer.first == stoi(p.payload))
                {
                    prot_msg ack(msg_id++, id, relayer.second, 0, 1, to_string(relayer.first));
                    mysend(ack);
                }
            }
            p.print();
            cout << "ack" << endl;
        }
        //nack received
        if (p.func_id == 2)
        {
            cout << "nack" << endl;
        }
        //discover received
        if (p.func_id == 8)
        {

            handle_discover(p);
        }
        //route received - a path returned
        else if (p.func_id == 16)
        {
            handle_route(p);
        }
        //send received
        else if (p.func_id == 32)
        {
            //if im the dest
            if (p.dest_id == this->id)
            {
                //if the sender of this massage isn't a sibling - send ack to the sender
                if (sib.count(p.src_id) == 0)
                {
                    //search for the relaying node
                    for (auto &relayer : relayed)
                    {
                        if (relayer.msg_id + 1 == p.msg_id)
                        {
                            int dest = stoi(relayer.payload.substr(0, 4));
                            prot_msg ack(msg_id++, id, dest, 0, 1, to_string(relayer.msg_id));
                            mysend(ack);
                            break;
                        }
                    }
                }
                //if the sender is my sibling - send ack back straight
                else
                {
                    prot_msg msg(msg_id++, id, p.src_id, 0, 1, to_string(p.msg_id));
                    mysend(msg);
                }
            }

            else
            {
                this->relay(p);
            }
        }
        else if (p.func_id == 64)
        {
            ack_to_relay.push_back({p.msg_id, stoi(p.payload.substr(0, 4))});
            relayed.push_back(p);
        }
        return 0;
    }
    ///////////////////////////////HANDLE DISCOVER///////////////
    int handle_discover(prot_msg p)
    {
        //if color=0 you open to discover.
        cout << p.payload << endl;
        int final_dest = stoi(p.payload);
        if (sib.find(final_dest) == sib.end())
        {
            send_dicovers(final_dest, "", p.src_id);
        }
        else
        {
            cout << "Path founded" << endl;
            //i find a way! lets tell him
            string s = addZero(p.msg_id) + addZero(2) + addZero(id) + addZero(final_dest);
            prot_msg ack_route(msg_id++, id, p.src_id, 0, 16, s);
            mysend(ack_route);
        }
        return 0;
    }

    void send_dicovers(int dest, string flag, int dad)
    {
        count_discovers[dest] = 0;
        temp_len[dest] = MAX;
        //dont look for wat throght me.
        for (auto const &x : sib)
        {
            if (x.first != dad)
            {
                prot_msg msg(msg_id++, id, x.first, 0, 8, addZero(dest));
                //only if send sucseed
                if (mysend(msg, x.second) != -1)
                {
                    count_discovers[dest] = count_discovers[dest] + 1;
                }
                //i need to know my when i started dicovers
                if (flag == "my")
                {
                    my_disco.push_back(msg.msg_id);
                }

                // cout<<"number of discover sent : "<<count_discovers<<endl;
                
            }
        }
    }
    //////////////////////////////BROADCAST///////////////
    void send_broadcast(prot_msg p)
    {
        for (auto const &x : sib)
        {
            if (x.first != p.src_id)
            {
                cout << "Send broadcast to all siblings..." << endl;
                //only if send sucseed
                if (mysend(p, x.second) != -1)
                {
                    cout << "Broadcast has been sent" << endl;
                }
            }
        }
    }
};

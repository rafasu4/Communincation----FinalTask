#ifndef __SELECT_H__
#define __SELECT_H__
#include "vector"
#include <map>

int add_fd_to_monitoring(const unsigned int fd);
int wait_for_input();
void split_str( std::string const &str, const char delim,  std::vector <std::string> &out );
int init();
void addZero(std::string& s, int i);
std::string addZero( int i);
int remove_zero_stoi(std::string s);
int remove_fd(int fd);
int dele_map(int someValue,std::map<int,int> someMap);
void peers(std::map<int,int> someMap);

//connect,127.0.0.1:5001
//setid,1
#endif
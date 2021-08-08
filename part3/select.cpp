#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>  
#include <vector>  
#include <sstream> 


using namespace std;
#define TRUE (1)
#define FALSE (0)

static fd_set rfds, rfds_copy;
static int max_fd = 0;
static int initialized = FALSE;
static int *alloced_fds = NULL;
static int alloced_fds_num = 0;

int remove_fd(int fd){
  cout<<"fd removed : "<<fd<<endl;
  for(int i=0; i<alloced_fds_num; ++i)
    {
      if ( alloced_fds[i] == fd)
        {
          for (int j  = i ; j < alloced_fds_num -1; j++){
                alloced_fds[j] = alloced_fds[j+1];
          }
          break;
        }
    }

  int *tmp_alloc;
  tmp_alloc = (int*)realloc(alloced_fds, sizeof(int)*(alloced_fds_num-1));
  if (tmp_alloc == NULL)
    return -1;

  alloced_fds = tmp_alloc;
  alloced_fds_num--;

  FD_CLR(fd, &rfds_copy);

  return 0;
}
static int add_fd_to_monitoring_internal(const unsigned int fd)
{
  //realloc place for 1 more monitoring
  int *tmp_alloc;
  
  tmp_alloc = (int*)realloc(alloced_fds, sizeof(int)*(alloced_fds_num+1));
  if (tmp_alloc == NULL)
    return -1;
  alloced_fds = tmp_alloc;
  alloced_fds[alloced_fds_num++]=fd;
  FD_SET(fd, &rfds_copy);
  if (max_fd < fd)
    max_fd = fd;

  return 0;
}

int init()
{
  FD_ZERO(&rfds_copy);
  if (add_fd_to_monitoring_internal(0) < 0)
    return -1; // monitoring standard input
  initialized = TRUE;
  return 0;
}

int add_fd_to_monitoring(const unsigned int fd)
{
  if (!initialized)
    init();
  if (fd>0)
    return add_fd_to_monitoring_internal(fd);
  return 0;
}

int wait_for_input()
{
  int i, retval;
  memcpy(&rfds, &rfds_copy, sizeof(rfds_copy));
  //retval checks if there is any msg waiting in the file descriptors 
  retval = select(max_fd+1, &rfds, NULL, NULL, NULL);
  if (retval > 0)
  {
    //lets search where the mesagge is  : )
    for (i=0; i<alloced_fds_num; ++i)
    {
      //cout<<"check if there mesggae in fd number:"<<alloced_fds[i]<<endl;
      if (FD_ISSET(alloced_fds[i], &rfds))
      //we found one ! lets check whats in it . 
        return alloced_fds[i];
    }
  }
  return -1;
}

//**************************************************some func****************************************************

void split_str( std::string const &str, const char delim,  std::vector <std::string> &out )  
        {  
            // create a stream from the string  
            std::stringstream s(str);  
              
            std::string s2;  
            while (getline (s, s2, delim) )  
            {  
                out.push_back(s2); // store the string in s2  
            }  
        
        }  

 //Calculates if and how many zeros need to be added for 4 byte length
    void addZero(string& s, int i)
    {
      
         if (i < 10)
        {
            s += "000";
        }
        else if (i < 100)
        {
            s += "00";
        }
        else if (i < 1000)
        {
            s += "0";
        }
        
    }

   
    string addZero(int i)
    {
      string s;
        if (i < 10)
        {
            s += "000";
        }
        else if (i < 100)
        {
            s += "00";
        }
        else if (i < 1000)
        {
            s += "0";
        }
    return s+to_string(i);
    }


int remove_zero_stoi(string s){
string ans="";
for (char c:s){
if (c!='0'){
ans=ans+c;
}
}
    

int a=stoi(ans);
return a;
}
int dele_map(int someValue,map<int,int> someMap){
for (auto it = someMap.begin(); it != someMap.end(); ++it){
    if (it->second == someValue){
        return it->first;
    }}
    return -1;
}
void peers(map<int,int> someMap){
for (auto it = someMap.begin(); it != someMap.end(); ++it){
  cout<<"nodeid: " <<it->first<<endl;
}
    
}
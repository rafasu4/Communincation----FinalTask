#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "node.cpp"



using namespace std;

int main (){
    node node;
    string input;
    while(true){
        cin >> input;    
        node.user_input(input);
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "node.cpp"

using namespace std;

int main()
{

    // cout<<"tetsting__________"<<endl;
    // map<int, prot_msg*> sent;
    // prot_msg a(1,1,1,1,1,"hey");
    // st[1]=&a;
    // sent[1]->print();
    node n1;
    // int temp;
    // cout << "Enter node ID: ";
    // cin >> temp;
    // n1.setid(temp);en
    string input;
    while(1){
        cin >> input;
        n1.user_input(input);
    }

    return 0;
}
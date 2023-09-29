#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

//Code is provided by Dr. Rincon from Canvas

struct threader //for threading
{
    // vector<pair<char, int>> entVec; //the map containing the data for the CPU
    string inpStr; //the full unedited string 
    // int ord; //the number of the thread (and CPU)
    string outStr; //the final entropy string
    char* serverIP; // argv[1]
    char* portno; //argv[2]
    threader(/*vector<pair<char, int>> eV,*/ string iS, /*int o,*/ char* sIP, char* p) { //constructor
        // entVec = eV;
        inpStr = iS;
        // ord = o;
        serverIP = sIP;
        portno = p;
    }
};

void* threadInstruct(void* arg)
{
    //input
    struct sockaddr_in serv_addr;
    struct hostent *server;
    threader* arg_ptr = (threader*) arg;
    int portno = atoi(arg_ptr->portno);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    string buffer = arg_ptr->inpStr;
    if (sockfd < 0)
    {
        cerr << "ERROR opening socket" << endl;
        exit(0);
    }
    server = gethostbyname(arg_ptr->serverIP);
    if (server == NULL)
    {
        cerr << "ERROR, no such host" << endl;
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) { cerr << "ERROR connecting" << endl; exit(0); }
    //output
    int msgSize = buffer.size();
    if (write(sockfd,&msgSize,sizeof(int)) < 0)  { cerr << "ERROR writing size to socket" << endl; exit(0); }
    if (write(sockfd,buffer.c_str(),msgSize) < 0) { cerr << "ERROR writing string to socket" << endl; exit(0); }
    if (read(sockfd,&msgSize,sizeof(int)) < 0) { cerr << "ERROR reading size from socket" << endl; exit(0); }
    char *tempBuffer = new char[msgSize+1];
    bzero(tempBuffer,msgSize+1);
    if (read(sockfd,tempBuffer,msgSize) < 0) { cerr << "ERROR reading string from socket" << endl; exit(0); }
    buffer = tempBuffer;
    arg_ptr->outStr = buffer;
    delete [] tempBuffer;
    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[])
{
    string input = "";
    vector<string> cpuCounter;
    vector<pthread_t> threadVector;
    vector<threader*> structVector;
    if (argc != 3) { cerr << "usage " << argv[0] << " hostname port" << endl; exit(0); }
    // cout << "Please enter the message: ";
    // getline(cin,buffer);
    while(getline(cin, input))
    {
        if(input.empty()) { break; }
        cpuCounter.push_back(input);
    }
    //pthread create
    for(int a = 0; a < cpuCounter.size(); a++) //variable-A threads
    {
        threader* newThread = new threader(/*allThreads[a],*/cpuCounter[a], /*a,*/ argv[1], argv[2]);
        pthread_t myThread;
        if(pthread_create(&myThread, NULL, threadInstruct, static_cast<void*> (newThread)))
        {
            cout << "ERROR";
            return -1;
        }
        structVector.push_back(newThread);
        threadVector.push_back(myThread);
    }
    //pthread join
    for(int b = 0; b < threadVector.size(); b++) { pthread_join(threadVector[b], NULL); }
    //output
    for(int c = 0; c < structVector.size(); c++) { cout << structVector[c]->outStr << endl << endl; }
    return 0;
}

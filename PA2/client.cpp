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

using namespace std;

struct threader //for threading
{
    vector<pair<char, int>> entVec; //the map containing the data for the CPU
    string inpStr; //the full unedited string 
    int ord; //the number of the thread (and CPU)
    string outStr; //the final entropy string
    char* serverIP; // argv[1]
    int portno; //argv[2]
    threader(vector<pair<char, int>> eV, string iS, int o, char* sIP, int p) { //constructor
        entVec = eV;
        inpStr = iS;
        ord = o;
        serverIP = sIP;
        portno = p;
    }
};

void* threadInstruct(void* arg)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    threader* arg_ptr = (threader*) arg;
    portno = arg_ptr->portno;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cerr << "ERROR opening socket" << endl;
        exit(0);
    }
    server = gethostbyname(arg_ptr->serverIP);
    if (server == NULL) {
        cerr << "ERROR, no such host" << endl;
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        cerr << "ERROR connecting" << endl;
        exit(0);
    }
    //
    int msgSize = sizeof(buffer);
    if (write(sockfd,&msgSize,sizeof(int))) 
    {
        cerr << "ERROR writing to socket" << endl;
        exit(0);
    }
    if (write(sockfd,buffer.c_str(),msgSize)) 
    {
        cerr << "ERROR writing to socket" << endl;
        exit(0);
    }
    if (read(sockfd,&msgSize,sizeof(int))) 
    {
        cerr << "ERROR reading from socket" << endl;
        exit(0);
    }
    char *tempBuffer = new char[msgSize+1];
    bzero(tempBuffer,msgSize+1);
    n = read(sockfd,tempBuffer,msgSize);
    if (n < 0) 
    {
        cerr << "ERROR reading from socket" << endl;
        exit(0);
    }
    buffer = tempBuffer;
    delete [] tempBuffer;
    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[])
{
    // int sockfd, portno, n;
    string input = "", buffer = "";
    vector<string> cpuCounter;
    vector<pthread_t> threadVector;
    vector<threader*> structVector;
    vector<vector<pair<char, int>>> allThreads;
    if (argc != 3) 
    {
       cerr << "usage " << argv[0] << " hostname port" << endl;
       exit(0);
    }
    // cout << "Please enter the message: ";
    // getline(cin,buffer);
    while(getline(cin, input))
    {
        if(input.empty()) { break; }
        buffer = buffer + input + '\n';
        cpuCounter.push_back(input);
    }
    //pthread create
    for(int a = 0; a < cpuCounter.size(); a++) //variable-A threads
    {
        threader* newThread = new threader(allThreads[a], cpuCounter[a], a, argv[1], argv[2]);
        pthread_t myThread;
        if(pthread_create(&myThread, NULL, threadInstruct, static_cast<void*> (newThread)))
        {
            cout << "ERROR";
            return -1;
        }
        pointerVector.push_back(newThread);
        threadVector.push_back(myThread);
    }
    //pthread join
    for(int b = 0; b < threadVector.size(); b++) { pthread_join(threadVector[b], NULL); }
    //output
    cout << "Message from server: "<< buffer << endl;
    return 0;
}

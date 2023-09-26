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
    vector<string> outVec; //the final entropy vector
    threader(vector<pair<char, int>> eV, string iS, int o) { //constructor
        entVec = eV;
        inpStr = iS;
        ord = o;
    }
};

//dont forget to call
//pthread create and join

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    string buffer = "";
    vector<string> bufferVector;
    vector<pthread_t> threadVector;
    vector<threader*> structVector;
    vector<vector<pair<char, int>>> allThreads;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc != 3) 
    {
       cerr << "usage " << argv[0] << " hostname port" << endl;
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        cerr << "ERROR opening socket" << endl;
        exit(0);
    }
    server = gethostbyname(argv[1]);
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
    // cout << "Please enter the message: ";
    // getline(cin,buffer);
    while(getline(cin, buffer))
    {
        if(buffer.empty()) { break; }
        bufferVector.push_back(buffer);
        stringstream s(buffer);
        vector<pair<char,int>> entropyVector;
        char toFind = ' '; 
        int toCalc = 0;
        while(s >> toFind >> toCalc) { entropyVector.push_back(make_pair(toFind, toCalc)); }
        allThreads.push_back(entropyVector);
    }
    int msgSize = sizeof(buffer);
    n = write(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
    {
        cerr << "ERROR writing to socket" << endl;
        exit(0);
    }
    n = write(sockfd,buffer.c_str(),msgSize);
    if (n < 0) 
    {
        cerr << "ERROR writing to socket" << endl;
        exit(0);
    }
    n = read(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
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
    cout << "Message from server: "<< buffer << endl;
    close(sockfd);
    return 0;
}
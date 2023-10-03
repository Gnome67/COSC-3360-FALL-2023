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

void fireman(int) { while (waitpid(-1, NULL, WNOHANG) > 0) {;} }

vector<double> calculateEntropy(vector<pair<char, int>> entropyVector)
{
   vector<double> entropyHold;
   char selectedTask = ' '; //the current task we are selecting the entropy for
   int extraFreq = 0; //the frequencies paired with the selectedTask
   double currTerm = 0.00; //currentTerm = freq[selectedTask] * log2(freq[selectedTask])
   double newTerm = 0.00; //newTerm = (freq[selectedTask] + extraFreq) * log2(freq[selectedTask] + extraFreq)
   int currFreq = 0; //the sum of all previous frequencies
   double currEntropy = 0.00; //initally 0, converts to the most recently created entropy
   double entropy = 0.00; //the entropy for the selectedTask and the extraFrequency
   int nFreq = 0; //the sum of all previous frequencies + the frequency paired with the selectedTask
   unordered_map<char, int> freqArray; //the frequency Array
   for(const auto &it : entropyVector) { freqArray.insert({it.first, 0}); }
   for(int x = 0; x < entropyVector.size(); x++) //for each task
   {
      selectedTask = entropyVector[x].first; //initialize selectedTask
      extraFreq = entropyVector[x].second; //initialize extraFrequency
      //begin the algorithm
      nFreq = currFreq + extraFreq;
      if(nFreq == extraFreq) { entropy = 0; } //for the first loop, the entropy will always be 0
      else
      {
         if( freqArray[selectedTask] == 0) { currTerm = 0; }
         else { currTerm = freqArray[selectedTask] * log2(freqArray[selectedTask]); }
         newTerm = (freqArray[selectedTask] + extraFreq) * log2(freqArray[selectedTask] + extraFreq);
         entropy = log2(nFreq) - ((log2(currFreq) - currEntropy) * currFreq - currTerm + newTerm)/nFreq;
      }
      entropyHold.push_back(entropy);
      currFreq += extraFreq;
      freqArray[selectedTask] += extraFreq;
      currEntropy = entropy;
   }
   return entropyHold;
}

string output(vector<pair<char, int>> entropyVector, string CPUcount, int cpu)
{
   string outputString = "CPU " + to_string(cpu), entropyString = "";
   outputString += "\nTask scheduling information: ";
   stringstream s(CPUcount);
   char x; int y;
   while(s >> x >> y) { outputString = outputString + x + "(" + to_string(y) + "), "; }
   outputString.pop_back(); //remove ending space
   outputString.pop_back(); //remove ending comma
   outputString += "\nEntropy for CPU " + to_string(cpu)+"\n";
   vector<double> answer = calculateEntropy(entropyVector);
   ostringstream entropyStream;
   for(const double& num : answer) { entropyStream << fixed << setprecision(2) << num << " "; }
   outputString += entropyStream.str();
   outputString.pop_back();
   return outputString;
}

int main(int argc, char *argv[])
{
   int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   // Check the commandline arguments
   if (argc != 2) { cerr << "Port not provided" << endl; exit(0); }
   // Create the socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) { cerr << "Error opening socket" << endl; exit(0); }
   // Populate the sockaddr_in structure
   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   // Bind the socket with the sockaddr_in structure
   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { cerr << "Error binding" << endl; exit(0); }
   // Set the max number of concurrent connections
   listen(sockfd, 5);
   clilen = sizeof(cli_addr);
   signal(SIGCHLD, fireman);
   int counter = 0; 
   while(true)
   {
      // Accept a new connection
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
      counter++;
      if(fork() == 0)
      {
         if (newsockfd < 0) { cerr << "Error accepting new connections" << endl; exit(0); }
         int msgSize = 0;
         if (read(newsockfd, &msgSize, sizeof(int)) < 0) { cerr << "Error reading size from socket" << endl; exit(0); }
         char *tempBuffer = new char[msgSize + 1];
         bzero(tempBuffer, msgSize + 1);
         if (read(newsockfd, tempBuffer, msgSize + 1) < 0) { cerr << "Error reading string from socket" << endl; exit(0); }
         string buffer = tempBuffer;
         delete[] tempBuffer;
         stringstream b(buffer);
         vector<pair<char, int>> entropyVector;
         char x; int y;
         while(b >> x >> y) { entropyVector.push_back(make_pair(x, y)); }
         // cout << "Message from client: " << buffer << ", Message size: " << msgSize << endl;
         string newBuffer = output(entropyVector, buffer, counter);
         msgSize = newBuffer.size();
         if (write(newsockfd, &msgSize, sizeof(int)) < 0) { cerr << "Error writing size to socket" << endl; exit(0); }
         if (write(newsockfd, newBuffer.c_str(), msgSize) < 0) { cerr << "Error writing string to socket" << endl; exit(0); }
      }
      _exit(0);
   }
   close(newsockfd);
   close(sockfd);
   return 0;
}

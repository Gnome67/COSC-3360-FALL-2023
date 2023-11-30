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
#include <pthread.h>
#include <unistd.h>

using namespace std;

struct threader //for threading
{
    vector<pair<char, int>> entVec; //the map containing the data for the CPU
    string inpStr; //the full unedited string 
    int ord; //the number of the thread (and CPU)
    int *counter; //the variable that ensures we print in order
    pthread_mutex_t *mutex;
    pthread_mutex_t *mutex2;
    pthread_cond_t *condition;
    threader() {}
};

//Function for calculating the entropy as specified in the algorithm

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

//The threaded function telling the threads what to do

void* threadInstruct(void* arg)
{
    threader threadArg = *(threader*) arg;
    vector<pair<char,int>> localEntropy = threadArg.entVec;
    string localCPUcount = threadArg.inpStr;
    int localCPU = threadArg.ord;
    pthread_mutex_unlock(threadArg.mutex);
    string outputString = "CPU " + to_string(localCPU+1);
    outputString += "\nTask scheduling information: ";
    stringstream s(localCPUcount);
    char x; int y;
    while(s >> x >> y) { outputString = outputString + x + "(" + to_string(y) + "), "; }
    outputString.pop_back(); //remove ending space
    outputString.pop_back(); //remove ending comma
    outputString += "\nEntropy for CPU " + to_string(localCPU+1)+"\n";
    vector<double> answer = calculateEntropy(localEntropy);
    ostringstream entropyStream;
    for(const double& num : answer) { entropyStream << fixed << setprecision(2) << num << " "; }
    outputString += entropyStream.str();
    pthread_mutex_lock(threadArg.mutex2);
    while(*threadArg.counter!=localCPU) { pthread_cond_wait(condition, mutex2); }
    pthread_mutex_unlock(threadArg.mutex2);
    cout << outputString << endl << endl;
    pthread_mutex_lock(threadArg.mutex2);
    (*threadArg.counter)++;
    pthread_cond_broadcast(threadArg.condition);
    pthread_mutex_unlock(threadArg.mutex2);
    return NULL;
}

int main () 
{
    vector<string> cpuCounter; //keeps track of how many threads to create, and contains the raw input strings
    string inputN = ""; //the input string
    vector<pthread_t> threadVector; //vector of the threads, since we don't know how many threads there will be
    vector<vector<pair<char, int>>> allThreads; //multiple strings means multiple vector pairs
    pthread_mutex_t mutex; //initialize first semaphore
    pthread_mutex_init(&mutex, nullptr);
    pthread_mutex_t mutex2; //initialize second semaphore
    pthread_mutex_init(&mutex2, nullptr);
    pthread_cond_t condition = PTHREAD_COND_INITIALIZER; //initialize first conditional variable
    static int counter = 0; //keep track of what order we print
    //process input
    while(getline(cin, inputN))
    {
        if(inputN.empty()) { break; }
        cpuCounter.push_back(inputN);
        stringstream s(inputN);
        vector<pair<char,int>> entropyVector;
        char toFind = ' '; 
        int toCalc = 0;
        while(s >> toFind >> toCalc) { entropyVector.push_back(make_pair(toFind, toCalc)); }
        allThreads.push_back(entropyVector);
    }
    //create threads
    threader newThread; //only using 1 struct
    //initializing struct constants
    newThread.counter = &counter;
    newThread.mutex = &mutex;
    newThread.condition = &condition;
    newThread.mutex2 = &mutex2;
    for(int a = 0; a < cpuCounter.size(); a++) //variable-A threads
    {
        pthread_t myThread;
        pthread_mutex_lock(&mutex);
        newThread.entVec = allThreads[a];
        newThread.inpStr = cpuCounter[a];
        newThread.ord = a;
        if(pthread_create(&myThread, NULL, threadInstruct, &newThread))
        {
            cout << "ERROR";
            return -1;
        }
        threadVector.push_back(myThread);
    }
    // join threads
    for(int b = 0; b < threadVector.size(); b++) { pthread_join(threadVector[b], NULL); }
    return 0;
}

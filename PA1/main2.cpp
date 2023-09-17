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
    vector<string> outVec; //the final entropy vector
    threader(vector<pair<char, int>> eV, string iS, int o) { //constructor
        entVec = eV;
        inpStr = iS;
        ord = o;
    }
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
    unordered_map<char, int> keyIndices; //for keys with multiple values, keep track of which value we are on
    for(const auto &it : entropyVector) //construct freqAarray and keyIndices
    {
        freqArray.insert({it.first, 0});
        keyIndices.insert({it.first, 0});
    }
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

/*
CPU 1
Task scheduling information: A(2), B(4), C(3), A(7)
Entropy for CPU 1
0.00 0.92 1.53 1.42
*/

//Handles the output format as specified in instructions

vector<string> output(vector<pair<char, int>> entropyVector, string CPUcount, int cpu)
{
    stringstream s(CPUcount);
    vector<string> outputVector;
    string newString = "";
    char x;
    int y;
    while(s >> x >> y)
    {
        newString += x;
        newString += "(";
        newString += to_string(y);
        newString += "), ";
    }
    newString.pop_back(); //there will be a space at the end, this is to take care of that
    newString.pop_back(); //there will then be a comma, remove that as well
    outputVector.push_back("CPU " + to_string(cpu+1));
    outputVector.push_back("Task scheduling information: " + newString);
    outputVector.push_back("Entropy for CPU " + to_string(cpu+1));
    vector<double> answer = calculateEntropy(entropyVector);
    for(int counterTwo = 0; counterTwo < answer.size(); counterTwo++) { outputVector.push_back(to_string(answer[counterTwo])); }
    return outputVector;
}

//The threaded function telling the threads what to do

void* threadInstruct(void* arg)
{
    threader* threadArg = (threader*) arg;
    threadArg->outVec = output(threadArg->entVec, threadArg->inpStr, threadArg->ord);
    return NULL;
}

int main () 
{
    vector<string> cpuCounter; //keeps track of how many threads to create, and contains the raw input strings
    string inputN = ""; //the input string
    vector<pthread_t> threadVector; //vector of the threads, since we don't know how many threads there will be
    vector<threader*> pointerVector; //vector of structs so each thread has its own struct
    vector<vector<pair<char, int>>> allThreads; //multiple strings means multiple vector pairs
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
    for(int a = 0; a < cpuCounter.size(); a++) //variable-A threads
    {
        threader* newThread = new threader(allThreads[a], cpuCounter[a], a);
        pthread_t myThread;
        if(pthread_create(&myThread, NULL, threadInstruct, static_cast<void*> (newThread)))
        {
            cout << "ERROR";
            return -1;
        }
        pointerVector.push_back(newThread);
        threadVector.push_back(myThread);
    }
    // join threads
    for(int c = 0; c < threadVector.size(); c++) { pthread_join(threadVector[c], NULL); }
    //output information
    for(int d = 0; d < pointerVector.size()-1; d++)
    {
        for(int e = 0; e < 3; e++) { cout << pointerVector[d]->outVec[e] << endl; }
        for(int e = 3; e < pointerVector[d]->outVec.size()-1; e++) { cout << fixed << setprecision(2) << stod(pointerVector[d]->outVec[e]) << " "; }
        cout << fixed << setprecision(2) << stod(pointerVector[d]->outVec[pointerVector[d]->outVec.size()-1]) << endl << endl;
    }
    for(int e = 0; e < 3; e++) { cout << pointerVector[pointerVector.size()-1]->outVec[e] << endl; }
    for(int e = 3; e < pointerVector[pointerVector.size()-1]->outVec.size()-1; e++) { cout << fixed << setprecision(2) << stod(pointerVector[pointerVector.size()-1]->outVec[e]) << " "; }
    cout << fixed << setprecision(2) << stod(pointerVector[pointerVector.size()-1]->outVec[pointerVector[pointerVector.size()-1]->outVec.size()-1]);
    return 0;
}
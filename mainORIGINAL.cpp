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
    unordered_map<char, vector<int>> entMap; //the map containing the data for the CPU
    string tasOrd; //the order that this specific threaded CPU will follow
    string inpStr; //the full unedited string 
    int ord; //the number of the thread (and CPU)
    vector<string> outVec; //the final entropy vector
    threader(unordered_map<char, vector<int>> eM, string tO, string iS, int o) { //constructor
        entMap = eM;
        tasOrd = tO;
        inpStr = iS;
        ord = o;
    }
};

//Function for calculating the entropy as specified in the algorithm

vector<float> calculateEntropy(unordered_map<char, vector<int>> entropyMap, string taskOrder)
{
    vector<float> entropyHold;
    char selectedTask = ' '; //the current task we are selecting the entropy for
    int extraFreq = 0; //the frequencies paired with the selectedTask
    float currTerm = 0.00; //currentTerm = freq[selectedTask] * log2(freq[selectedTask])
    float newTerm = 0.00; //newTerm = (freq[selectedTask] + extraFreq) * log2(freq[selectedTask] + extraFreq)
    int currFreq = 0; //the sum of all previous frequencies
    float currEntropy = 0.00; //initally 0, converts to the most recently created entropy
    float entropy = 0.00; //the entropy for the selectedTask and the extraFrequency
    int nFreq = 0; //the sum of all previous frequencies + the frequency paired with the selectedTask
    unordered_map<char, int> freqArray; //the frequency Array
    unordered_map<char, int> keyIndices; //for keys with multiple values, keep track of which value we are on
    for(const auto &it : entropyMap) //construct freqAarray and keyIndices
    {
        freqArray.insert({it.first, 0});
        keyIndices.insert({it.first, 0});
    }
    for(int x = 0; x < taskOrder.size(); x++) //for each task
    {
        selectedTask = taskOrder[x];
        //follow the tasks in the order they were given, below iterator is for if a task letter is repeated with multiple frequencies and to count how many times a repeated task shows up, then find out which iteration of the repeated task we are on, to calculate extraFreq
        unordered_map<char, vector<int>>::iterator iterator = entropyMap.find(selectedTask);
        if(iterator != entropyMap.end() && iterator->second.size() > 1)
        { // e.g. A has values 2, 7
            vector<int> indices = iterator->second; // {2, 7}
            int index = keyIndices[selectedTask]; //0 returns 2, 1 returns 7
            extraFreq = indices[index]; //0 returns 2, 1 returns 7
            keyIndices[selectedTask]++; //update the index by 1 for future calls
        }
        else { extraFreq = iterator->second[0]; } //if there is only 1 value in the task's vector, then just put that as the current task's frequency
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

vector<string> output(unordered_map<char, vector<int>> entropyMap, string taskOrder, string CPUcount, int cpu)
{
    vector<string> outputVector;
    string newString = "";
    for(int a = 0; a < CPUcount.size(); a += 4)
    {
        if(isalpha(CPUcount[a]))
        {
            newString += CPUcount[a];
            newString += "(";
            newString += CPUcount[a+2];
            newString += ")";
            newString += ", ";
        }
    }
    newString.pop_back(); //there will be a space at the end, this is to take care of that
    newString.pop_back(); //there will then be a comma, remove that as well
    outputVector.push_back("CPU " + to_string(cpu+1));
    outputVector.push_back("Task scheduling information: " + newString);
    outputVector.push_back("Entropy for CPU " + to_string(cpu+1));
    vector<float> answer = calculateEntropy(entropyMap, taskOrder);
    for(int counterTwo = 0; counterTwo < answer.size(); counterTwo++) { outputVector.push_back(to_string(answer[counterTwo])); }
    return outputVector;
}

//The threaded function telling the threads what to do

void* threadInstruct(void* arg)
{
    threader* threadArg = (threader*) arg;
    //TODO do the thing
    threadArg->outVec = output(threadArg->entMap, threadArg->tasOrd, threadArg->inpStr, threadArg->ord);
    return NULL;
}

int main () 
{
    char toFind = ' '; 
    int toCalc = 0;
    vector<string> cpuCounter; 
    string inputN = "";
    vector<string> taskOrderCount; 
    vector<unordered_map<char, vector<int>>> allThreads;
    vector<pthread_t> threadVector;
    vector<threader*> pointerVector;
    while(true)
    {
        getline(cin, inputN);
        if(inputN.empty()) { break; }
        cpuCounter.push_back(inputN);
        string taskOrder = ""; 
        unordered_map<char, vector<int>> entropyMap; 
        for(int x = 0; x < inputN.length(); x += 4)
        {
            if(isalpha(inputN[x]) && x + 1 < inputN.length())
            {
                toFind = inputN[x];
                taskOrder += toFind;
                if(x + 2 < inputN.length()) { toCalc = inputN[x+2] - '0'; }
                else { break; }
            }
            if(entropyMap.find(toFind) != entropyMap.end())
            {
                entropyMap[toFind].push_back(toCalc);
            }
            else
            {
                entropyMap.insert({toFind, {toCalc}});
            }
        }
        allThreads.push_back(entropyMap);
        taskOrderCount.push_back(taskOrder);
    }
    for(int a = 0; a < cpuCounter.size(); a++) //variable-A threads
    {
        threader* newThread = new threader(allThreads[a], taskOrderCount[a], cpuCounter[a], a);
        pthread_t myThread;
        if(pthread_create(&myThread, NULL, threadInstruct, static_cast<void*> (newThread)))
        {
            cout << "ERROR";
            return -1;
        }
        pointerVector.push_back(newThread);
        threadVector.push_back(myThread);
    }
    // if(cpuCounter.size() > 1)
    // {
    //     int a = 0;
    //     for(a = 0; a < cpuCounter.size() - 1; a++)
    //     {
    //         output(allThreads[a], taskOrderCount[a], cpuCounter[a], a);
    //         cout << endl << endl;
    //     }
    //     output(allThreads[a], taskOrderCount[a], cpuCounter[a], a);
    // }
    // else { output(allThreads[0], taskOrderCount[0], cpuCounter[0], 0); }
    for(int c = 0; c < threadVector.size(); c++)
    {
        pthread_join(threadVector[c], NULL);
    }
    for(int d = 0; d < pointerVector.size()-1; d++)
    {
        for(int e = 0; e < 3 /*pointerVector[d]->outVec.size()*/; e++)
        {
            cout << pointerVector[d]->outVec[e] << endl;
        }
        for(int e = 3; e < pointerVector[d]->outVec.size()-1; e++)
        {
            cout << pointerVector[d]->outVec[e].substr(0, 4) << " ";
        }
        cout << pointerVector[d]->outVec[pointerVector[d]->outVec.size()-1].substr(0, 4);
        cout << endl << endl;
    }
    for(int e = 0; e < 3; e++)
    {
        cout << pointerVector[pointerVector.size()-1]->outVec[e] << endl;
    }
    for(int e = 3; e < pointerVector[pointerVector.size()-1]->outVec.size()-1; e++)
    {
        cout << pointerVector[pointerVector.size()-1]->outVec[e].substr(0, 4) << " ";
    }
    cout << pointerVector[pointerVector.size()-1]->outVec[pointerVector[pointerVector.size()-1]->outVec.size()-1].substr(0, 4);

    //freeing up memory
    inputN.clear();
    for(int x = 0; x < allThreads.size(); x++)
    {
        allThreads[x].clear();
    }
    allThreads.clear();
    allThreads.shrink_to_fit();
    cpuCounter.clear();
    cpuCounter.shrink_to_fit();
    taskOrderCount.clear();
    taskOrderCount.shrink_to_fit();
    threadVector.clear();
    threadVector.shrink_to_fit();
    for(threader* threaderPtr : pointerVector)
    {
        delete threaderPtr;
    }
    pointerVector.clear();
    pointerVector.shrink_to_fit();
    return 0;
}

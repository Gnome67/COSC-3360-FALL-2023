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

struct thread //for threading
{
    unordered_map<char, vector<int>> entMap; //the map containing the data for the CPU
    string tasOrd; //the order that this specific threaded CPU will follow
};

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
    unordered_map<char, int> freqArray;
    unordered_map<char, int> keyIndices;
    for(const auto &it : entropyMap)
    {
        freqArray.insert({it.first, 0});
        keyIndices.insert({it.first, 0});
    }
    for(int x = 0; x < taskOrder.size(); x++) //for each task
    {
        //follow the tasks in the order they were given, below for loop is for if a task letter is repeated with multiple frequencies
        selectedTask = taskOrder[x];
        // count how many times a repeated task shows up, then find out which iteration of the repeated task we are on, to calculate extraFreq
        unordered_map<char, vector<int>>::iterator iterator = entropyMap.find(selectedTask);
        if(iterator != entropyMap.end() && iterator->second.size() > 1)
        {
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

void output(unordered_map<char, vector<int>> entropyMap, string taskOrder, string CPUcount, int cpu)
{
    string newString = "";
    for(int a = 0; a < CPUcount.size(); a++)
    {
        if(isalpha(CPUcount[a]))
        {
            newString += CPUcount[a];
            newString += "(";
            newString += CPUcount.substr(a + 2, 1);
            newString += ")";
            newString += ", ";
        }
    }
    newString.pop_back(); //there will be a space at the end, this is to take care of that
    newString.pop_back(); //there will then be a comma, remove that as well
    cout << "CPU " << cpu+1 << endl;
    cout << "Task scheduling information: " << newString << endl;
    cout << "Entropy for CPU " << cpu+1 << endl;
    vector<float> answer = calculateEntropy(entropyMap, taskOrder);
    for(int counterTwo = 0; counterTwo < answer.size() - 1; counterTwo++) { cout << fixed << setprecision(2) << answer[counterTwo] << " "; }
    cout << answer[answer.size()-1];
}

int main () 
{
    char toFind = ' '; 
    int toCalc = 0; 
    char currFind = ' '; 
    int currCalc = 0; 
    vector<string> cpuCounter; 
    string inputN = "";
    vector<string> taskOrderCount; 
    vector<unordered_map<char, vector<int>>> allThreads;
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
    if(cpuCounter.size() > 1)
    {
        int a = 0;
        for(a = 0; a < cpuCounter.size() - 1; a++)
        {
            output(allThreads[a], taskOrderCount[a], cpuCounter[a], a);
            cout << endl << endl;
        }
        output(allThreads[a], taskOrderCount[a], cpuCounter[a], a);
    }
    else { output(allThreads[0], taskOrderCount[0], cpuCounter[0], 0); }
    return 0;
}
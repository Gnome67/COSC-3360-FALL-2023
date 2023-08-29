#include <iostream>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <unistd.h>
#include <math.h>

using namespace std;

struct threader
{

};

/*
Output Formatting:
(ex. string:
A 2 B 4 C 3 A 7
B 3 A 3 C 3 A 1 B 1 C 1
)

CPU 1
Task scheduling information: A(2), B(4), C(3), A(7)
Entropy for CPU 1
0.00 0.92 1.53 1.42

CPU 2
Tak scheduling information: B(3), A(3), C(3), A(1), B(1), C(1)
Entropy for CPU 2
0.00 1.00 1.58 1.57 1.57 1.58
*/

float calculateEntropy(unordered_map<char, vector<int>> entropyMap, string taskOrder)
{
    char selectedTask = ' '; //the current task we are selecting the entropy for
    int extraFreq = 0; //the frequencies paired with the selectedTask
    float currTerm = 0.00; //currentTerm = freq[selectedTask] * log2(freq[selectedTask])
    float newTerm = 0.00; //newTerm = (freq[selectedTask] + extraFreq) * log2(freq[selectedTask] + extraFreq)
    int currFreq = 0; //the sum of all previous frequencies
    float currEntropy = 0.00; //initally 0, converts to the most recently created entropy
    float entropy = 0.00; //the entropy for the selectedTask and the extraFrequency
    int nFreq = currFreq + extraFreq; //the sum of all previous frequencies + the frequency paired with the selectedTask
    //Formula = log2(NFreq) - ((log2(currFreq) - currH) * (currFreq) - currentTerm + newTerm)/NFreq
    for(int x = 0; x < taskOrder.size(); x++) //for each task
    {
        //follow the tasks in the order they were given, below for loop is for if a task letter is repeaated with multiple frequencies
        for(int y = 0; y < entropyMap[entropyMap.find(taskOrder[x])].size(); y++) //if a task has more than 1 frequency
        {
            // calculate entropy for 2nd task
        }
    }
    if(nFreq == extraFreq) { entropy = 0; } 
    else { entropy = log2(nFreq) - ((log2(currFreq - currEntropy) * (currFreq) - currTerm + newTerm)/nFreq); }
    return entropy;
}

void output(int CPUcount)
{
    for(int counter = 0; counter < CPUcount; counter++)
        {
            cout << "CPU " << counter << endl;
            cout << "Task scheduling information: " << "This is not ready yet." << endl;
            cout << "Entropy for CPU " << counter << endl;
            cout << "Replace this with a function to calculate/store entropy values." << endl;
        }
}

int main () 
{
    char toFind = ' '; //the character
    int toCalc = 0; //the units of time for the character
    char currFind = ' '; //the current character we are calculating the entropy for
    int currCalc = 0; //the current units of time we are calculating the entropy for

    vector<string> cpuCounter; //list of how many threads to create
    string inputN = ""; //the moodle STDIN string
    string toSplit = ""; //how we find each key and value by using substring
    string taskOrder = ""; //the order in which we will perform the tasks
    vector<int> holding; //pushing each char and its frequency into a map, but since the map is a char and vector, we need a vector to hold the frequencies temporarily
    
    //make a map of chars and vectors
    //each STRING is a thread, find a way to separate the tasks in all strings from all other strings
    unordered_map<char, vector<int>> entropyMap;
    do
        {
            cin >> inputN;
            cpuCounter.push_back(inputN);
            //group up the letters and their frequencies
            for(int x = 0; x < inputN.length(); x++)
            {
                    if(isalpha(inputN[x])) //if this is a valid task
                    {
                        toFind = inputN[x];
                        //taskOrder keeps task of what tasks we have so far
                        taskOrder += toFind;
                        //find the frequency (toCalc) for the task (toSplit)
                        toSplit = inputN.substr(toFind+2,1);
                        //convert toSplit (char) to toCalc (int)
                        toCalc = toSplit - '0';
                    }
                    //check if the value is in the map already
                    if(entropyMap.contains(toFind))
                    {
                        //if it is, add it to the key's value-vector
                        entropyMap[toFind].push_back(toCalc);
                    }
                    else
                    {
                        //otherwise add a new key and value
                        holding.push_back(toCalc);
                        entropyMap.insert(toFind, holding);
                        holding.pop_back();
                    }
            }
        }
    while(inputN != "");
    //for each INDIVIDUAL value in the map, calculate entropy
    output(cpuCounter.size());
}

//Entropy algorithm in pseudocode
/*
Input: freq, currFreq, currH, selectedTask, extraFreq
Output: H, NFreq

1. NFreq = currFreq + extraFreq
2. if NFreq == extraFreq then
3.     H = 0
4. else
5.     if freq[selectedTask] == 0 then
6.         currentTerm = 0
7.     else
8.         currentTerm = freq[selectedTask] * log2(freq[selectedTask])
9.     end if
10.         newTerm = (freq[selectedTask] + extraFreq) * log2(freq[selectedTask] + extraFreq)
11.     H = log sub2(NFreq) - ((log sub2(currFreq) - currH) * (currFreq) - currentTerm + newTerm)/NFreq
12. end if
13. return H, NFreq
*/

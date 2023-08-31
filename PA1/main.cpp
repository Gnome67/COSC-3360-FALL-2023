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

struct threader //for threading
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
    int nFreq = /*currFreq + extraFreq*/ 0; //the sum of all previous frequencies + the frequency paired with the selectedTask
    //Formula = log2(NFreq) - ((log2(currFreq) - currH) * (currFreq) - currentTerm + newTerm)/NFreq

    unordered_map<char, int> frequencyArray;
    for(int count = 0; count < entropyMap.size(); count++;)
    {
        frequencyArray.insert({entropyMap[count], 0}) //initializing the frequency array
    }
    
    /*
    Steps
    1. initialize the selectedTask and extraFrequency DONE?
    2. initialize nFreq DONE
    3. initialize H if it hasnt already been initialized DONE
    4. set currFreq += extraFreq DONE
    5. set currTerm = freq[x];
    6. set newTerm = (freq[x] + extraFreq)*log(freq[x]+extraFreq)
    LOOP X+1
    7. set H = log2(nFreq) - ((log2(currFreq - currEntropy) * (currFreq) - currTerm + newTerm)/nFreq)
    8. 
    */

    for(int x = 0; x < taskOrder.size(); x++) //for each task
    {
        //follow the tasks in the order they were given, below for loop is for if a task letter is repeated with multiple frequencies
        selectedTask = taskOrder[x];
        // count how many times a repeated task shows up, then find out which iteration of the repeated task we are on, to calculate extraFreq
        int iterator = entropyMap.find(selectedTask);
        if(entropyMap[iterator].size() > 1)
        {
            //find which iteration of the repeated task we are on
            int counter = count(taskOrder.begin(), taskOrder.end(), taskOrder[x]); //keep track of how many times the repeated frequency shows up
            string tempString = taskOrder;
            while(counter != 0)
            {
                if(tempString.find(tempString[x]) == x)
                {
                    //if the index of the current task matches the find() index, then we have found our extraFreq
                    int freqValue = tempString.find(tempString[x]);
                    extraFreq = entropyMap[freqValue];
                    counter = 0;
                }
                else
                {
                    tempString.substr(tempString.find(tempString[x])+1);
                    counter--;
                }
            }
        }
        else { extraFreq = entropyMap[0]; } //if there is only 1 value in the task's vector, then just put that as the current task's frequency
        nFreq = currFreq + extraFreq;
        //impement currentTerm
        if(frequencyArray[x].second == 0) { currentTerm = 0; }
        else { currentTerm = frequencyArray[x].second * log2(; }
        //implement newTerm
        newTerm = (frequencyArray[x] + extraFreq) * log2(frequencyArray[x] + extraFreq);
        if(nFreq == extraFreq) { entropy = 0; } //for the first loop, the entropy will always be 0
        else { entropy = log2(nFreq) - ((log2(currFreq - currEntropy) * (currFreq) - currTerm + newTerm)/nFreq); }
        currFreq += extraFreq;
        //implement freqArray
        frequencyArray[x].second = extraFreq;
    }
    
    return entropyHold;
}

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

void output(string CPUcount, vector<float> answer)
{
    string newString = "";
    //separate the letters and the numbers
    //put the numbers in () with a comma and space after
    for(int a = 0; a < CPUcount.size(); a++)
    {
        if(isalpha(CPUcount[a]))
        {
            newString += CPUcount[a];
            newString += "(";
            newString += CPUcount.substr(CPUcount[a]+2, 1);
            newString += ")";
            newString += ",";
        }
    }
    newString.pop_back(); //there will be a comma at the end, this is to take care of that
    for(int b = 0; b < CPUcount.size(); b++)
        {
            cout << "CPU " << counter << endl;
            cout << "Task scheduling information: " << newString << endl;
            cout << "Entropy for CPU " << counter << endl;
            for(int c = 0; c < CPUcount.size()-1; c++)
            {
                cout << answer[c] << " ";
            }
            cout << answer[answer.size() - 1];
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
    vector<string> taskOrderCount; //this holds the taskOrder for each CPU
    vector<int> holding; //pushing each char and its frequency into a map, but since the map is a char and vector, we need a vector to hold the frequencies temporarily
    
    //make a map of chars and vectors
    //each STRING is a thread, find a way to separate the tasks in all strings from all other strings
    unordered_map<char, vector<int>> entropyMap; //for each individual threaded CPU
    vector<unordered_map<char, vector<int>>> allThreads; //holds all threaded CPUs

    //make this a while loop that initializes a new map when a string is inputted and processes the string into a map
    //once the string is processed, push the map into the vector of maps, and initialize the next map
    do
        {
            getline(cin, inputN);
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
                        toCalc = stoi(toSplit);
                    }
                    //check if the key is in the map already
                    if(entropyMap.find(toFind) != entropyMap.end())
                    {
                        //if it is, add the corresponding value to the key's value-vector
                        entropyMap[toFind].push_back(toCalc);
                    }
                    else
                    {
                        //otherwise add a new key and value
                        holding.push_back(toCalc);
                        entropyMap.insert({toFind, holding});
                        holding.pop_back();
                    }
            }
            allThreads.push_back(entropyMap);
            taskOrderCount.push_back(taskOrder);
            entropyMap.clear();
            taskOrderCount.pop_back();
        }
    while(inputN != "");
    //for each INDIVIDUAL value in the map, calculate entropy
    vector<float> answer = calculateEntropy(allThreads[0], taskOrderCount[0]); //use a for loop to loop through each thread
    output(cpuCounter[0], answer); //use a for loop to loop through each thread
}

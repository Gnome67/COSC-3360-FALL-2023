#include <iostream>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <vector>

using namespace std;

int main () 
{
    char toFind = ""; //the character
    int toCalc = 0; //the units of time for the character
    char currFind = ""; //the current character we are calculating the entropy for
    int currCalc = 0; //the current units of time we are calculating the entropy for
    
    string inputN = " "; //the moodle STDIN string
    string toSplit = ""; //how we find each key and value by using substring
    vector<int> holding; //pushing each char and its frequency into a map, but since the map is a char and vector, we need a vector to hold the frequencies temporarily
    
    //make a map of chars and vectors
    unordered_map<char, vector<int>> entropyMap;
    do
        {
            cin >> inputN;
            //group up the letters and their frequencies
            for(int x = 0; x < inputN.length(); x++)
            {
                    if(isalpha(inputN[x]))
                    {
                        toFind = inputN[X];
                        toSplit = inputN.substr(toFind+2,1);
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
    while(inputN != "")
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
8.         currentTerm = freq[selectedTask] * log sub-b(freq[selectedTask])
9.     end if
10.         newTerm = (freq[selectedTask] + extraFreq) * log sub-b(freq[selectedTask] + extraFreq)
11.     H = log sub-b (NFreq) - ((log sub-b(currFreq) - currH) * (currFreq) - currentTerm + newTerm)/NFreq
12. end if
13. return H, NFreq
*/

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
    char toFind = "";
    int toCalc = "";
    string inputN = " ";
    string toSplit = "";
    //make a map of chars and vectors
    unordered_map<char, vector<int>> entropyMap;
    do
        {
            cin >> inputN;
            //group up the letters and their frequencies
            for(int x = 0; x < inputN.length(); x++)
                {
                    if(isalpha(inputN[x])
                    {
                        toFind = inputN[X];
                        toSplit = inputN.substr(toFind+2,1);
                    }
                    //check if the value is in the map already
                    if(entropyMap.contains(toFind))
                    {
                        //if it is, add it to the key's value-vector   
                    }
                    else
                    {
                        //otherwise add a new key and value
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

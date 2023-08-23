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
    vector<string> strings;
    string inputN = "";
    while(inputN != "")
    {
        cin >> inputN;
        strings.push_back(inputN);
    }
    for(int counter = 0; counter < strings.size(); counter++)
    {
        //group up the letters and their frequencies
    }
}

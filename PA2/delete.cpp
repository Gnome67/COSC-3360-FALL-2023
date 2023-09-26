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

void* threadInstruct(void* arg)
{
    threader* threadArg = (threader*) arg;
    threadArg->outVec = output(threadArg->entVec, threadArg->inpStr, threadArg->ord);
    return NULL;
}

int main () 
{
    for(int d = 0; d < pointerVector.size()-1; d++)
    {
        for(int e = 0; e < 3; e++) { cout << pointerVector[d]->outVec[e] << endl; }
        for(int e = 3; e < pointerVector[d]->outVec.size()-1; e++) { cout << fixed << setprecision(2) << stod(pointerVector[d]->outVec[e]) << " "; }
        cout << fixed << setprecision(2) << stod(pointerVector[d]->outVec[pointerVector[d]->outVec.size()-1]) << endl << endl;
    }
    for(int e = 0; e < 3; e++) { cout << pointerVector[pointerVector.size()-1]->outVec[e] << endl; }
    for(int e = 3; e < pointerVector[pointerVector.size()-1]->outVec.size()-1; e++) { cout << fixed << setprecision(2) << stod(pointerVector[pointerVector.size()-1]->outVec[e]) << " "; }
    cout << fixed << setprecision(2) << stod(pointerVector[pointerVector.size()-1]->outVec[pointerVector[pointerVector.size()-1]->outVec.size()-1]);
}
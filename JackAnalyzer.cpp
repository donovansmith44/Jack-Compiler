#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

using namespace std;

vector<string> getJackFiles(string directoryName)
{
    vector<string> files;

    string path(directoryName);

    string ext(".jack");

    for(auto &p : filesystem::recursive_directory_iterator(path))
    {
        if(p.path().extension() == ext)
        {
            files.push_back(directoryName + "/" + p.path().stem().string() + ".jack");
        }
    }
    return files;
}

string getJackFileName(string jackFile)
{
    if (jackFile.find('/') != string::npos)
    {
        jackFile.erase(0, jackFile.find('/')+1);
    }
    if (jackFile.find('.') != string::npos)
    {
        jackFile.erase(jackFile.find('.'), jackFile.length() - jackFile.find('.'));
    }
    return jackFile;
}

int main(int argc, char* argv[]){
    vector<string> jackFiles = getJackFiles(argv[1]);
    ofstream vmFile;
    ifstream jackFile;
    for (int i = 0; i < jackFiles.size(); i++)
    {
        jackFile.open(jackFiles[i]);
        CompilationEngine parser(jackFile, getJackFileName(jackFiles[i]) + ".vm");
        //VMWriter myVMWriter(getJackFileName(jackFiles[i]) + ".vm");
        parser.compileClass();
        jackFile.close();
    }

    return 0;
}
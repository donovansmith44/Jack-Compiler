#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

using namespace std;

class SymbolTable
{
    public:
    SymbolTable();
    void startSubroutine(string);
    void Define(string, string, string);
    int VarCount(string);
    string KindOf(string);
    string TypeOf(string);
    int IndexOf(string);

    private:
    int numArgs;
    int numFields;
    int numStatics;
    int numVars;
    unordered_map<string, tuple<string, string, int>> classScopeVariables;
    unordered_map<string, tuple<string, string, int>> subroutineScopeVariables;
};

#endif
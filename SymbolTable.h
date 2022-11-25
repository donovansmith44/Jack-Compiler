#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

using namespace std;

class SymbolTable
{
    public:
    SymbolTable();
    void startSubroutine();
    void Define(string, string, string);
    int VarCount(string);
    string KindOf(string);
    string TypeOf(string);
    int IndexOf(string);

    private:

};
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include "SymbolTable.h"
using namespace std;

    SymbolTable::SymbolTable()
    {
        
    }
    void SymbolTable::startSubroutine()
    {
        return;
    }
    void SymbolTable::Define(string name, string type, string kind)
    {
        return;
    }
    int SymbolTable::VarCount(string kind)
    {
        return 0;
    }
    string SymbolTable::KindOf(string name)
    {
        return "";
    }
    string SymbolTable::TypeOf(string name)
    {
        return "";
    }
    int SymbolTable::IndexOf(string name)
    {
        return 0;
    }

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include "SymbolTable.h"

using namespace std;

    SymbolTable::SymbolTable()
    {
        numArgs = 0;
        numFields = 0;
        numStatics = 0;
        numVars = 0;
    }
    void SymbolTable::startSubroutine(string className)
    {
        numArgs = 0;
        numVars = 0;
        subroutineScopeVariables.clear();
        subroutineScopeVariables["this"] = make_tuple(className, "argument", 0); //get<0>(tuple) = className
        numArgs++;
        return;
    }
    void SymbolTable::Define(string name, string type, string kind)
    {
        if (kind == "STATIC")
        {
            classScopeVariables[name] = make_tuple(type, kind, numStatics);
            numStatics++;
        }
        else if(kind == "FIELD")
        {
            classScopeVariables[name] = make_tuple(type, kind, numFields);
            numFields++;
        }
        else if(kind == "ARG")
        {
            subroutineScopeVariables[name] = make_tuple(type, kind, numArgs);
            numArgs++;
        }
        else if(kind == "VAR")
        {
            subroutineScopeVariables[name] = make_tuple(type, kind, numVars);
            numVars++;
        }
        return;
    }
    int SymbolTable::VarCount(string kind)
    {
        if (kind == "STATIC")
        {
            return numStatics;
        }
        else if(kind == "FIELD")
        {
            return numFields;
        }
        else if(kind == "ARG")
        {
            return numArgs;
        }
        else if(kind == "VAR")
        {
            return numVars;
        }
        return 0;
    }
    string SymbolTable::TypeOf(string name)
    {
        if (!get<0>(classScopeVariables[name]).empty())
        {
            return get<0>(classScopeVariables[name]);
        }
        else if(!get<0>(subroutineScopeVariables[name]).empty())
        {
            return get<0>(subroutineScopeVariables[name]);
        }
        else
        {
            return "NONE";
        }
    }
    string SymbolTable::KindOf(string name)
    {
        if (!get<1>(classScopeVariables[name]).empty())
        {
            return get<1>(classScopeVariables[name]);
        }
        else if(!get<1>(subroutineScopeVariables[name]).empty())
        {
            return get<1>(subroutineScopeVariables[name]);
        }
        else
        {
            return "NONE";
        }
    }
    int SymbolTable::IndexOf(string name)
    {
        if (get<2>(classScopeVariables[name]) != 0)
        {
            return get<2>(classScopeVariables[name]);
        }
        else if(get<2>(subroutineScopeVariables[name]) != 0)
        {
            return get<2>(subroutineScopeVariables[name]);
        }
        
        return 0;
    }

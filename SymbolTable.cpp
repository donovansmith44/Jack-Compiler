#include <iostream>
#include <string>
#include <iomanip>
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
    void SymbolTable::startSubroutine()
    {
        numArgs = 0;
        numVars = 0;
        subroutineScopeVariables.clear();
        return;
    }
    void SymbolTable::Define(string name, string type, string kind)
    {   
        if(kind == "static")
        {
            classScopeVariables[name] = make_tuple(type, "static", numStatics);
            numStatics++;
        }
        else if(kind == "field")
        {
            classScopeVariables[name] = make_tuple(type, "field", numFields);
            numFields++;
        }
        else if(kind == "ARG")
        {
            subroutineScopeVariables[name] = make_tuple(type, "argument", numArgs);
            numArgs++;
        }
        else if(kind == "VAR")
        {
            subroutineScopeVariables[name] = make_tuple(type, "local", numVars);
            numVars++;
        } 
        else
        {
            return;
        }

        return;
    }
    int SymbolTable::VarCount(string kind)
    {
        if(kind == "static")
        {
            return numStatics;
        }
        else if(kind == "field")
        {
            return numFields;
        }
        else if(kind == "argument")
        {
            return numArgs;
        }
        else if(kind == "local")
        {
            return numVars;
        }
        else
        {
            return 0;
        }
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
        else
        {
            return 0;
        }
    }

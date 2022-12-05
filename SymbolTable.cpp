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
        switch (kind)
        {
        case "STATIC":
            classScopeVariables[name] = make_tuple(type, kind, numStatics);
            numStatics++;
            break;

        case "FIELD":
            classScopeVariables[name] = make_tuple(type, kind, numFields);
            numFields++;
            break;

        case "ARG":
            subroutineScopeVariables[name] = make_tuple(type, kind, numArgs);
            numArgs++;
            break;

        case "VAR":
            subroutineScopeVariables[name] = make_tuple(type, kind, numVars);
            numVars++;
            break;

        default:
            return;
            break;
        }
    }
    int SymbolTable::VarCount(string kind)
    {
        switch (kind)
        {
        case "STATIC":
            return numStatics;
            break;

        case "FIELD":
            return numFields;
            break;

        case "ARG":
            return numArgs;
            break;

        case "VAR":
            return numVars;
            break;

        default:
            return 0;
            break;
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

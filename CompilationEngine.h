#ifndef COMPILATION_ENGINE
#define COMPILATION_ENGINE

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

using namespace std;

class CompilationEngine{
    public:
    CompilationEngine();
    CompilationEngine(ifstream&, string);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
    private:
    ifstream _jackInput;
    JackTokenizer myTokenizer;
    SymbolTable mySymbolTable;
    VMWriter myVMWriter;
    string thisClass;
    string currentFunction;
    vector<string> currentSymbols;
    string currentSubroutineReturnType;
    int numArgs;
    int whileLabels;
    int ifLabels;
};


#endif

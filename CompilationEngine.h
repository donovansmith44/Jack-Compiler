#ifndef COMPILATION_ENGINE
#define COMPILATION_ENGINE

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "JackTokenizer.h"

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
    ofstream _xmlOutput;
    JackTokenizer myTokenizer;
};


#endif

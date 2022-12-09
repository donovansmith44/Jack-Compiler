#ifndef VMWRITER_H
#define VMWRITER_H

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

class VMWriter
{
    public:
    VMWriter();
    VMWriter(string);
    void writePush(string, int);
    void writePop(string, int);
    void writeArithmetic(string);
    void writeLabel(string);
    void writeGoto(string);
    void writeIf(string);
    void writeCall(string, int);
    void writeFunction(string, int);
    void writeReturn();
    void close();

    private:
    string tolower(string);
    ofstream vmFile;
};
#endif
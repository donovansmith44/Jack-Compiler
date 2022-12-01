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

using namespace std;

class VMWriter
{
    public:
    VMWriter();
    VMWriter(ofstream);
    void writePush(string);
    void writePop(string);
    void writeArithmetic(string);
    void writeLabel(string);
    void writeGoto(string);
    void writeIf(string);
    void writeCall(string, int);
    void writeFunction(string, int);
    void writeReturn();
    void close();
};
#endif
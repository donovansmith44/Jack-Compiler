#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include "VMWriter.h"

using namespace std;

VMWriter::VMWriter()
{

}
VMWriter::VMWriter(string outFileName)
{
    vmFile.open(outFileName);
}
void VMWriter::writePush(string segment, int index)
{
    if (segment == "CONST")
    {
        vmFile << "push constant " << index << endl;
    }
    else if (segment == "argument")
    {
        vmFile << "push argument " << index << endl;
    }
    else if (segment == "local")
    {
        vmFile << "push local " << index << endl;
    }
    else if (segment == "static")
    {
        vmFile << "push static " << index << endl;
    }
    else if (segment == "THIS")
    {
        vmFile << "push this " << index << endl;
    }
    else if (segment == "THAT")
    {
        vmFile << "push that " << index << endl;
    }
    else if (segment == "POINTER")
    {
        vmFile << "push pointer " << index << endl;
    }
    else if (segment == "TEMP")
    {
        vmFile << "push temp " << index << endl;
    }
    
    return;
}
void VMWriter::writePop(string segment, int index)
{
    if (segment == "CONST")
    {
        vmFile << "pop constant " << index << endl;
    }
    else if (segment == "argument")
    {
        vmFile << "pop argument " << index << endl;
    }
    else if (segment == "local")
    {
        vmFile << "pop local " << index << endl;
    }
    else if (segment == "static")
    {
        vmFile << "pop static " << index << endl;
    }
    else if (segment == "THIS")
    {
        vmFile << "pop this " << index << endl;
    }
    else if (segment == "THAT")
    {
        vmFile << "pop that " << index << endl;
    }
    else if (segment == "POINTER")
    {
        vmFile << "pop pointer " << index << endl;
    }
    else if (segment == "TEMP")
    {
        vmFile << "pop temp " << index << endl;
    }

    return;
}
void VMWriter::writeArithmetic(string command)
{
    vmFile << tolower(command) << endl;
    return;
}
void VMWriter::writeLabel(string label)
{
    vmFile << "label " << label << endl;
    return;
}
void VMWriter::writeGoto(string label)
{
    vmFile << "goto " << label << endl;
    return;
}
void VMWriter::writeIf(string label)
{
    vmFile << "if-goto " << label << endl;
    return;
}
void VMWriter::writeCall(string name, int nArgs)
{
    vmFile << "call " << name << " " << nArgs << endl;
    return;
}
void VMWriter::writeFunction(string name, int nLocals)
{
    vmFile << "function " << name << " " << nLocals << endl;
    return;
}
void VMWriter::writeReturn()
{
    vmFile << "return" << endl;
    return;
}
string VMWriter::tolower(string stringToConvert)
{
    for (int i = 0; i < stringToConvert.length(); i++)
    {
        if (stringToConvert[i] < 97)
        {
            stringToConvert[i] += 32;
        }
    }
    return stringToConvert;
}
void VMWriter::close()
{
    vmFile.close();
    return;
}
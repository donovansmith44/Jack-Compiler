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
VMWriter::VMWriter(ofstream outFile)
{
    
}
void VMWriter::writePush(string segment)
{
    return;
}
void VMWriter::writePop(string segment)
{
    return;
}
void VMWriter::writeArithmetic(string command)
{
    return;
}
void VMWriter::writeLabel(string label)
{
    return;
}
void VMWriter::writeGoto(string label)
{
    return;
}
void VMWriter::writeIf(string label)
{
    return;
}
void VMWriter::writeCall(string name, int nArgs)
{
    return;
}
void VMWriter::writeFunction(string name, int nLocals)
{
    return;
}
void VMWriter::writeReturn()
{
    return;
}
void VMWriter::close()
{
    return;
}
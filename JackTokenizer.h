#ifndef JACK_TOKENIZER
#define JACK_TOKENIZER
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <vector>

using namespace std;

class JackTokenizer{
    public:
    JackTokenizer();
    JackTokenizer(ifstream&);
    bool hasMoreTokens();
    void advance();
    string tokenType();
    string keyWord();
    char symbol();
    string identifier();
    int intVal();
    string stringVal();
    void clean();
    vector<string> removeLineComments(vector<string>);
    vector<string> removeAPIComments(vector<string>);
    vector<string> removeBlockComments(vector<string>);
    vector<string> appendStringLiterals(vector<string>);
    bool isKeyword(string);
    bool isSymbol(string);
    bool isStringConstant(string);
    bool isIntConstant(string);
    vector<string> Tokenize(string);
    string getNextToken();
    private:
    ifstream jackFile;
    ofstream xmlFile;
    string currentToken;
    vector<string> jackLines;
    vector<string> jackTokens;
    int commandNum;
    string nextToken;
};


#endif
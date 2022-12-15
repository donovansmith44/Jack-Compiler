#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <cctype>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

using namespace std;

JackTokenizer::JackTokenizer()
{

}
JackTokenizer::JackTokenizer(ifstream &jackFile)
{
        string line;
        commandNum = 0;
        currentToken = "";
        nextToken = "";
        
        while(getline(jackFile, line)) //we store all of the lines of Jack code into a vector
        {
            jackLines.push_back(line);
        }

        clean(); //remove comments and whitespace from input stream, and parse all of the code into individual tokens
}

bool JackTokenizer::hasMoreTokens()
{
    if (commandNum < jackTokens.size())
    {
        return true;
    }
    return false;
}
void JackTokenizer::advance()
{
    currentToken = jackTokens[commandNum];
    commandNum++;

    if (hasMoreTokens()) //it's useful to have a string that holds the token after the current one when compiling particular syntactic elements
    {
        nextToken = jackTokens[commandNum];
    }
    return;
}
string JackTokenizer::getNextToken()
{
    return nextToken;
}
string JackTokenizer::tokenType()
{
    if (isKeyword(currentToken))
    {
        return "KEYWORD";
    }
    else if (isSymbol(currentToken))
    {
        return "SYMBOL";
    }
    else if (currentToken.find('"') != string::npos)
    {
        return "STRING_CONST";
    }
    else if(isIntConstant(currentToken))
    {
        return "INT_CONST";
    }
    else
    {
        return "IDENTIFIER";
    }
}
string JackTokenizer::keyWord()
{
    return currentToken;
}
char JackTokenizer::symbol()
{
    return currentToken[0];
}
string JackTokenizer::identifier()
{  
    return currentToken;
}
int JackTokenizer::intVal()
{
    return stoi(currentToken);
}
string JackTokenizer::stringVal()
{
    return currentToken.substr(1, currentToken.length()-2); //ignore double quotes
}
vector<string> JackTokenizer::removeLineComments(vector<string> tokens)
{
    vector<string> newTokens;
    for (int i = 0; i < tokens.size(); i++)
    {
        if(tokens[i].find("//") != string::npos)
        {
            tokens[i].erase(tokens[i].find("//"), tokens[i].length()-tokens[i].find("//")); //ignore all characters following "//" during compile time
        }
        if (!tokens[i].empty() && tokens[i] != " ")
        {
            newTokens.push_back(tokens[i]);
        }   
    }
    
    return newTokens;
}
vector<string> JackTokenizer::removeAPIComments(vector<string> tokens)
{
    vector<string> newTokens;

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].find("/**") != string::npos)
        {
            for (int j = i; j < tokens.size(); j++)
            {
                if (tokens[j].find("*/") == string::npos)
                {
                    tokens[j] = "";
                }
                else
                {
                    tokens[j].erase(0, tokens[j].find("*/")+2);
                    break;
                }
            }
        }
        if (!tokens[i].empty() && tokens[i] != " ")
        {
            newTokens.push_back(tokens[i]);
        }
    }

    return newTokens;
}
vector<string> JackTokenizer::removeBlockComments(vector<string> tokens)
{
    vector<string> newTokens;

    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].find("/*") != string::npos)
        {
            for (int j = i; j < tokens.size(); j++)
            {
                if (tokens[j].find("*/") == string::npos)
                {
                    tokens[j] = "";
                }
                else
                {
                    tokens[j].erase(0, tokens[j].find("*/")+2);
                    break;
                }
            }
        }
        if (!tokens[i].empty() && tokens[i] != " ")
        {
            newTokens.push_back(tokens[i]);
        }
    }
    
    return newTokens;
}
vector<string> JackTokenizer::removeComments(vector<string> tokens)
{
    /*There are three different types of comments to remove from the input stream*/
    tokens = removeLineComments(tokens);
    tokens = removeAPIComments(tokens);
    tokens = removeBlockComments(tokens);
    
    return tokens;
}
vector<string> JackTokenizer::appendStringLiterals(vector<string> tokens)
{
    string temp = "";
    string temp2 = "";

    for (int i = 0; i < tokens.size(); i++)
    {
        temp = tokens[i];
        temp2 = temp;
        
        if (temp.find('"') != string::npos) //if we find an opening double quote
        {
            temp2.erase(temp2.begin() + temp2.find('"'));
            if (temp2.find('"') != string::npos) //and a closing double quote
            {
                tokens[i] = temp; //store the entire string constant to the list of tokens
            }
            else
            {
                for (int j = i+1; j < tokens.size(); j++)
                {
                    if(tokens[j].find('"') != string::npos) //if we find the closing double quote, then we have found the end of the string constant.
                    {
                        temp += " " + tokens[j];
                        tokens[i] = temp;
                        tokens[j] = " ";
                        break;
                    }
                    else
                    {
                        temp += " " + tokens[j]; //append the next portion of the string constant
                        tokens[j] = " ";
                    }
                }
            }
        }
        temp = "";
        
    }
    return tokens;
}
bool JackTokenizer::isKeyword(string line)
{
    if (line == "class" || line == "constructor" || line == "function" || line == "method" || line == "field" || line == "static" || line == "var" || line == "int" || line == "char" || line == "boolean" || line == "void" || line == "true" || line == "false" || line == "null" || line == "this" || line == "let" || line == "do" || line == "if" || line == "else" || line == "while" || line == "return")
    {
        return true;
    }
    return false;
}
bool JackTokenizer::isSymbol(string line)
{
    if(line == "{" || line == "}" || line == "(" || line == ")" || line == "[" || line == "]" || line == "." || line == "," || line == ";" || line == "+" || line == "-" || line == "*" || line == "/" || line == "&" || line == "|" || line == "<" || line == ">" || line == "=" || line == "~")
    {
        return true;
    }
    return false;
}
bool JackTokenizer::isIntConstant(string line)
{
    for (int i = 0; i < line.length(); i++)
    {
        if (!isdigit(line[i])) //if any of the characters in the string aren't digits, the string isn't a number.
        {
            return false;
        }
    }
    return true;
}
void JackTokenizer::clean() //remove comments and unnecessary whitespace lines
{
    string allLines = "";

    jackLines = removeComments(jackLines);
    
    for (int i = 0; i < jackLines.size(); i++) //concatenate all of the cleaned lines into a single string to be split into unique tokens
    {
        allLines += jackLines[i];
    }
    
    jackTokens = Tokenize(allLines);
    
    return;
}

vector<string> JackTokenizer::Tokenize(string line)
{
    int i = 0;
    string tempString = "";
    vector<string> tokens;

    for (int i = 0; i < line.length(); i++)
    {
        if (!isSymbol(line.substr(i+1, 1)) && line.substr(i+1, 1) != " ") //if the i+1th character is not a symbol or space
        {
            if (!isspace(line[i])) //iteratively add all non space characters into a temporary string variable, and check if it becomes a unique token
            {
                tempString += line[i];

                if (isKeyword(tempString) && isspace(line[i+1]))
                {
                    tokens.push_back(tempString);
                    tempString = "";
                }
                else if(isSymbol(tempString))
                {
                    tokens.push_back(tempString);
                    tempString = "";
                }

            }
        }
        else //if the i+1th character is a symbol or space, then save all of the non-space characters up to that symbol as a new token.
        {
            tempString += line[i];

                if (tempString != " " && !tempString.empty())
                {
                    tokens.push_back(tempString);
                }

            tempString = "";
        }
        
    }
    
    /*each space following an opening double quote will mark 
    the beginning of a new word, which will be tokenized with 
    all following words until the end of the double quote*/
    tokens = appendStringLiterals(tokens); 

    for (int i = 0; i < tokens.size(); i++) //remove the blank elements from the vector
    {
        string temp = tokens[i];
        if (isspace(temp[0]) | temp == " ")
        {
            tokens.erase(tokens.begin() + i);
            i--;
        }    
    }
    
    return tokens;
}
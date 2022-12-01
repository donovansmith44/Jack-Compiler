#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

using namespace std;

    CompilationEngine::CompilationEngine(ifstream &jackInput, string xmlFileName)
    {
        myTokenizer = JackTokenizer(jackInput);
        _xmlOutput.open(xmlFileName);

        // mySymbolTable.Define("x", "int", "FIELD");
        // mySymbolTable.Define("y", "string", "FIELD");
        // mySymbolTable.Define("z", "char", "FIELD");
        // cout << mySymbolTable.KindOf("x") << endl;
        // cout << mySymbolTable.TypeOf("y") << endl;
        // cout << mySymbolTable.TypeOf("z") << endl;
        // cout << mySymbolTable.VarCount("FIELD") << endl;
        // cout << mySymbolTable.VarCount("STATIC") << endl;
        // cout << mySymbolTable.IndexOf("x") << endl;
        // cout << mySymbolTable.IndexOf("y") << endl;
        // cout << mySymbolTable.IndexOf("z") << endl;
    }
    void CompilationEngine::compileClass()
    {
        if (myTokenizer.hasMoreTokens()) //class declaration
        {
                myTokenizer.advance();
                _xmlOutput << "<class>" << endl;
                _xmlOutput << " <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
                myTokenizer.advance();
                _xmlOutput << " <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
                myTokenizer.advance();
                _xmlOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        }

        while (myTokenizer.hasMoreTokens())//compile classVarDec and subroutineDec
        {
            myTokenizer.advance();
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                if (myTokenizer.keyWord() == "static" | myTokenizer.keyWord() == "field")
                {
                    compileClassVarDec();
                }
                else if(myTokenizer.keyWord() == "constructor" | myTokenizer.keyWord() == "function" | myTokenizer.keyWord() == "method")
                {
                    compileSubroutine();
                }
            }            
        }

        _xmlOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        _xmlOutput << "</class>" << endl;
        _xmlOutput.close();
        return;
    }
    void CompilationEngine::compileClassVarDec()
    {
        _xmlOutput << "<classVarDec>" << endl;
        _xmlOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
        myTokenizer.advance();
        while(myTokenizer.symbol() != ';')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                _xmlOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                _xmlOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            }
            else if(myTokenizer.symbol() == ',')
            {
                _xmlOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
            }
            myTokenizer.advance();
        }
        _xmlOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        _xmlOutput << " </classVarDec>" << endl;
        return;
    }
    void CompilationEngine::compileSubroutine()
    {
        _xmlOutput << "<subroutineDec>" << endl;
        _xmlOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
        myTokenizer.advance();

        if(myTokenizer.tokenType() == "IDENTIFIER")
        {
            _xmlOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            myTokenizer.advance();
        }
        else if (myTokenizer.tokenType() == "KEYWORD")
        {
            _xmlOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            myTokenizer.advance();
        }

        _xmlOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
        myTokenizer.advance();

        _xmlOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        myTokenizer.advance();
        compileParameterList();

        _xmlOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;

        /*compile subroutine body*/
        _xmlOutput << "  <subroutineBody>" << endl;
        myTokenizer.advance();
        _xmlOutput << "   <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        
        myTokenizer.advance();

        while (myTokenizer.keyWord() == "var")
        {
            compileVarDec();
            myTokenizer.advance();
        }

        compileStatements();

        _xmlOutput << "   <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        _xmlOutput << "  </subroutineBody>" << endl;
        _xmlOutput << " </subroutineDec>" << endl;
        return;
    }
    void CompilationEngine::compileParameterList()
    {
        _xmlOutput << "   <parameterList> " << endl;
        while (myTokenizer.symbol() != ')')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                _xmlOutput << "    <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                _xmlOutput << "    <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            }
            else if(myTokenizer.symbol() == ',')
            {
                _xmlOutput << "    <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
            }
            myTokenizer.advance();
        }
        _xmlOutput << "   </parameterList> "<< endl;
        return;
    }
    void CompilationEngine::compileVarDec()
    {
        _xmlOutput << "    <varDec> " << endl;
        _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
        myTokenizer.advance();
        while (myTokenizer.symbol() != ';')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            }
            else if (myTokenizer.tokenType() == "SYMBOL")
            {
                _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
            }
            else if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                _xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            }
            myTokenizer.advance();
        }
        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        _xmlOutput << "    </varDec> " << endl;
        return;
    }
    void CompilationEngine::compileStatements()
    {
        _xmlOutput << "   <statements> " << endl;
        while (myTokenizer.symbol() != '}')
        {
            if (myTokenizer.keyWord() == "let") //the currentToken should be ';' after the let statement compiles
            {
                compileLet();
                myTokenizer.advance();
            }
            else if (myTokenizer.keyWord() == "if") //the currentToken should be the first token after the statement's closing '}' after the if statement compiles
            {
                compileIf();
                
            }
            else if (myTokenizer.keyWord() == "do") //the currentToken should be ';' after the do statement compiles
            {
                compileDo();
                myTokenizer.advance();
            }
            else if (myTokenizer.keyWord() == "while") //the currentToken should be '}' after the while statement compiles
            {
                compileWhile();
                myTokenizer.advance();
            }
            else if (myTokenizer.keyWord() == "return") //the currentToken should be ';' after the return statement compiles
            {
                compileReturn();
                myTokenizer.advance();
            }
            else if (myTokenizer.keyWord() == "var")
            {
                compileVarDec();
                myTokenizer.advance();
            }
            else
            {
                myTokenizer.advance();
            }
        }
        _xmlOutput << "   </statements> " << endl;
        return;
    }
    void CompilationEngine::compileDo() 
    {
        _xmlOutput << "    <doStatement> " << endl;
        
        _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

            /*SUBROUTINE CALL*/
            if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                _xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                myTokenizer.advance();
                
                if(myTokenizer.symbol() == '.') // '.' subRoutine Name '(' expressionList ')'
                {
                    _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                    myTokenizer.advance();

                    _xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                    myTokenizer.advance();
                }

                    _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                    myTokenizer.advance();
                    
                    compileExpressionList();

                    _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
                    myTokenizer.advance();
            }

            _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'
            
            _xmlOutput << "    </doStatement> " << endl;
        return;
    }
    void CompilationEngine::compileLet()
    {
        _xmlOutput << "    <letStatement> " << endl;
        
        _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();
            
        _xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
        myTokenizer.advance();

        if (myTokenizer.symbol() == '[')
        {
            _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '['
            myTokenizer.advance();

            compileExpression();
            myTokenizer.advance();
            
            _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ']'
            myTokenizer.advance();
        }
        
        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '='
        myTokenizer.advance();

        compileExpression();
        myTokenizer.advance();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        _xmlOutput << "    </letStatement> " << endl;
        return;
    }
    void CompilationEngine::compileWhile()
    {
        _xmlOutput << "    <whileStatement> " << endl;
        
        _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
        myTokenizer.advance();

        compileExpression();

        myTokenizer.advance();
        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'

        myTokenizer.advance();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'

        compileStatements();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
        
        _xmlOutput << "    </whileStatement> " << endl;
        return;
    }
    void CompilationEngine::compileReturn()
    {
        _xmlOutput << "    <returnStatement> " << endl;
        
        _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        if (myTokenizer.symbol() != ';')
        {
            compileExpression();
            myTokenizer.advance();    
        }

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        _xmlOutput << "    </returnStatement> " << endl;
        return;
    }
    void CompilationEngine::compileIf()
    {
        _xmlOutput << "    <ifStatement> " << endl;
        
        _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
        myTokenizer.advance();

        compileExpression();
        myTokenizer.advance();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
        myTokenizer.advance();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'
        myTokenizer.advance();

        compileStatements();

        _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
        myTokenizer.advance();

        if (myTokenizer.keyWord() == "else")
        {
            _xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
            myTokenizer.advance();
            
            _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'
            myTokenizer.advance();

            compileStatements();

            _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
            myTokenizer.advance();
        }

        _xmlOutput << "    </ifStatement> " << endl;
        return;
    }
    void CompilationEngine::compileExpression()
    {
        _xmlOutput << "      <expression> " << endl;
        
        compileTerm();
        
        if (myTokenizer.getNextToken() == "+" | myTokenizer.getNextToken() == "-" | myTokenizer.getNextToken() == "*" | myTokenizer.getNextToken() == "/" | myTokenizer.getNextToken() == "&" | myTokenizer.getNextToken() == "|" | myTokenizer.getNextToken() == "<" | myTokenizer.getNextToken() == ">" | myTokenizer.getNextToken() == "=")
        {
            myTokenizer.advance();
            if (myTokenizer.symbol() == '<')
            {
                _xmlOutput << "     <symbol> " <<  "&lt;" << " </symbol>"  << endl;
            }
            else if (myTokenizer.symbol() == '>')
            {
                _xmlOutput << "     <symbol> " <<  "&gt;" << " </symbol>"  << endl;
            }
            else if (myTokenizer.symbol() == '&')
            {
                _xmlOutput << "     <symbol> " <<  "&amp;" << " </symbol>"  << endl;
            }
            else
            {
                _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // op
            }
            
            myTokenizer.advance();
            compileTerm();   
        }
        _xmlOutput << "      </expression> " << endl;
        return;
    }

    void CompilationEngine::compileTerm()
    {
        string tempToken = "";
        _xmlOutput << "       <term> " << endl;
        if (myTokenizer.tokenType() == "INT_CONST")
        {
            _xmlOutput << "      <integerConstant> " << myTokenizer.intVal() << " </integerConstant> " << endl;
        }
        else if (myTokenizer.tokenType() == "STRING_CONST")
        {
            tempToken = myTokenizer.stringVal();
            if(tempToken.find('"') != string::npos) //string constants are identified with a opening and closing '"'. Remove them for compilation
            {
                tempToken.erase(tempToken.find('"'));
            }
            if(tempToken.find('"') != string::npos)
            {
                tempToken.erase(tempToken.find('"'));
            }
            _xmlOutput << "      <stringConstant> " << tempToken << " </stringConstant> " << endl;
        }
        else if (myTokenizer.keyWord() == "true" | myTokenizer.keyWord() == "false" | myTokenizer.keyWord() == "null"  | myTokenizer.keyWord() == "this" )
        {
            _xmlOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            _xmlOutput << "      <identifier> " << myTokenizer.identifier() << " </identifier> " << endl;

            if (myTokenizer.getNextToken() == "[" | myTokenizer.getNextToken() == "(") //array or variable
            {
                myTokenizer.advance();
                
                _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl; // '(' | '['
                myTokenizer.advance();

                compileExpression();

                myTokenizer.advance();
                
                _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl; // ')' | ']'
            }
            else if(myTokenizer.getNextToken() == ".") //subroutineCall
            {
                myTokenizer.advance();
                _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                myTokenizer.advance();

                _xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                myTokenizer.advance();
            

                _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                myTokenizer.advance();
                
                compileExpressionList();

                _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
            }
        }
        else if (myTokenizer.symbol() == '-' || myTokenizer.symbol() == '~') //unaryop term
        {
            _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            myTokenizer.advance();
            compileTerm();
        }
        else if (myTokenizer.symbol() == '(') //'(' expression ')'
        {
            _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            myTokenizer.advance();
            compileExpression();
            myTokenizer.advance();
            _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
        }        
        _xmlOutput << "       </term>" << endl;
        
        return;
    }
    void CompilationEngine::compileExpressionList()
    {
        _xmlOutput << "       <expressionList> " << endl;
        if (myTokenizer.symbol() != ')')
        {
            compileExpression();
            myTokenizer.advance();
           
           while (myTokenizer.symbol() != ')')
            {
                if (myTokenizer.symbol() == ',')
                {
                    _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
                    myTokenizer.advance();
                    compileExpression();
                    myTokenizer.advance();
                }   
            }
        }
        _xmlOutput << "      </expressionList>" << endl;
        return;
    }
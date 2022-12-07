#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include "CompilationEngine.h"
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

using namespace std;

    CompilationEngine::CompilationEngine(ifstream &jackInput, string vmFileName)
    {
        myTokenizer = JackTokenizer(jackInput);
        myVMWriter = VMWriter(vmFileName);
        mySymbolTable = SymbolTable();

        _xmlOutput.open(vmFileName);
        thisClass = "";
        currentFunction = "";
        subroutineType = "";
        currentIdentifier = "";
        currentSymbol = "";
        currentSymbols.clear();
    }
    void CompilationEngine::compileClass()
    {
        if (myTokenizer.hasMoreTokens()) //class declaration
        {
                myTokenizer.advance(); //the first token in the tokenizer should be 'class'
                //_xmlOutput << "<class>" << endl;
                //_xmlOutput << " <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;

                myTokenizer.advance(); //token should now be className
                thisClass = myTokenizer.keyWord(); //setting thisClass to the current class name for reference in subroutine symbol tables
                //_xmlOutput << " <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;


                myTokenizer.advance(); //{
                //_xmlOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
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

        //_xmlOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_xmlOutput << "</class>" << endl;
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
        subroutineType = myTokenizer.keyWord();
        
        //_xmlOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl; //constructor, function, or method
        myTokenizer.advance();

        if(myTokenizer.tokenType() == "IDENTIFIER") //the function/constructor will have a return type of 'identifier' 
        {
            //_xmlOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            myTokenizer.advance();
        }
        else if (myTokenizer.tokenType() == "KEYWORD") //the function/constructor will have a predefined return type (int, void, char...)
        {
            //_xmlOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            myTokenizer.advance();
        }

        //_xmlOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl; //name of the function
        currentFunction = myTokenizer.identifier();

        myTokenizer.advance();

        //_xmlOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl; //(
        myTokenizer.advance();
        
        compileParameterList();

        //_xmlOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl; //)

        if (subroutineType == "method")
        {
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("ARG") + 1);
            myVMWriter.writePush("ARG", 0);
            myVMWriter.writePop("POINTER", 0);
        }
        else if(subroutineType == "function" || subroutineType == "constructor")
        {
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("ARG"));
        }
        else
        {
            /*this should not be possible.*/            
        }

        /*compile subroutine body*/
        //_xmlOutput << "  <subroutineBody>" << endl;
        myTokenizer.advance();
        //_xmlOutput << "   <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        
        myTokenizer.advance();

        while (myTokenizer.keyWord() == "var")
        {
            compileVarDec();
            myTokenizer.advance();
        }

        compileStatements();

        //_xmlOutput << "   <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_xmlOutput << "  </subroutineBody>" << endl;
        //_xmlOutput << " </subroutineDec>" << endl;
        return;
    }
    void CompilationEngine::compileParameterList()
    {
        //_xmlOutput << "   <parameterList> " << endl;
        while (myTokenizer.symbol() != ')')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                //_xmlOutput << "    <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_xmlOutput << "    <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            }
            else if(myTokenizer.symbol() == ',')
            {
                //_xmlOutput << "    <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
            }
            myTokenizer.advance();
        }
        //_xmlOutput << "   </parameterList> "<< endl;
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
        //_xmlOutput << "   <statements> " << endl;
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
      //  _xmlOutput << "   </statements> " << endl;
        return;
    }
    void CompilationEngine::compileDo() 
    {
        //_xmlOutput << "    <doStatement> " << endl;
        
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl; 'do'
        myTokenizer.advance();

            /*SUBROUTINE CALL*/
            if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                currentIdentifier = myTokenizer.identifier();
                myTokenizer.advance();
                
                if(myTokenizer.symbol() == '.') // '.' subRoutine Name '(' expressionList ')'
                {
                    //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                    myTokenizer.advance();

                    currentFunction = myTokenizer.identifier();
                    //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                    myTokenizer.advance();
                }

                    //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                    myTokenizer.advance();
                    
                    compileExpressionList();

                    //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
                    myTokenizer.advance();
                    myVMWriter.writeCall(currentIdentifier + "." + currentFunction, 1);
            }

            //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'
            
            //_xmlOutput << "    </doStatement> " << endl;
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
        //_xmlOutput << "    <returnStatement> " << endl;
        
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        if (myTokenizer.symbol() != ';')
        {
            compileExpression();
            myTokenizer.advance();    
        }

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        //_xmlOutput << "    </returnStatement> " << endl;
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
       // _xmlOutput << "      <expression> " << endl;
        
        compileTerm();
        
        if (myTokenizer.getNextToken() == "+" | myTokenizer.getNextToken() == "-" | myTokenizer.getNextToken() == "*" | myTokenizer.getNextToken() == "/" | myTokenizer.getNextToken() == "&" | myTokenizer.getNextToken() == "|" | myTokenizer.getNextToken() == "<" | myTokenizer.getNextToken() == ">" | myTokenizer.getNextToken() == "=")
        {
            currentSymbols.push_back(myTokenizer.getNextToken());
            myTokenizer.advance();
            //currentSymbol = myTokenizer.symbol();
            if (myTokenizer.symbol() == '<')
            {
         //       _xmlOutput << "     <symbol> " <<  "&lt;" << " </symbol>"  << endl;
            }
            else if (myTokenizer.symbol() == '>')
            {
         //       _xmlOutput << "     <symbol> " <<  "&gt;" << " </symbol>"  << endl;
            }
            else if (myTokenizer.symbol() == '&')
            {
         //       _xmlOutput << "     <symbol> " <<  "&amp;" << " </symbol>"  << endl;
            }
            else
            {
         //       _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // op
            }
            
            myTokenizer.advance();
            compileTerm();
            if (currentSymbols.back() == "+")
            {
                myVMWriter.writeArithmetic("ADD");
            }
            else if (currentSymbols.back() == "-")
            {
                myVMWriter.writeArithmetic("SUB");
            }
            else if (currentSymbols.back() == "*")
            {
                myVMWriter.writeCall("Math.multiply", 2);
            }
            else if (currentSymbols.back() == "/")
            {
                myVMWriter.writeCall("Math.divide", 2);
            }
            else if (currentSymbols.back() == "&")
            {
                myVMWriter.writeArithmetic("AND");
            }
            else if (currentSymbols.back() == "|")
            {
                myVMWriter.writeArithmetic("OR");
            }
            else if (currentSymbols.back() == "<")
            {
                myVMWriter.writeArithmetic("LT");
            }
            else if (currentSymbols.back() == ">")
            {
                myVMWriter.writeArithmetic("GT");
            }
            else if (currentSymbols.back() == "=")
            {
                myVMWriter.writeArithmetic("EQ");
            }
            currentSymbols.pop_back();
        }
        //_xmlOutput << "      </expression> " << endl;
        return;
    }

    void CompilationEngine::compileTerm()
    {
        string tempToken = "";
        //_xmlOutput << "       <term> " << endl;
        if (myTokenizer.tokenType() == "INT_CONST")
        {
          //  _xmlOutput << "      <integerConstant> " << myTokenizer.intVal() << " </integerConstant> " << endl;
          myVMWriter.writePush("CONST", myTokenizer.intVal());
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
           // _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            myTokenizer.advance();
            compileTerm();
        }
        else if (myTokenizer.symbol() == '(') //'(' expression ')'
        {
         //   _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            myTokenizer.advance();
            compileExpression();
            myTokenizer.advance();
           // _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
        }        
       // _xmlOutput << "       </term>" << endl;
        
        return;
    }
    void CompilationEngine::compileExpressionList()
    {
        //_xmlOutput << "       <expressionList> " << endl;
        if (myTokenizer.symbol() != ')')
        {
            compileExpression();
            myTokenizer.advance();
           
           while (myTokenizer.symbol() != ')')
            {
                if (myTokenizer.symbol() == ',')
                {
          //          _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
                    myTokenizer.advance();
                    compileExpression();
                    myTokenizer.advance();
                }   
            }
        }
        //_xmlOutput << "      </expressionList>" << endl;
        return;
    }
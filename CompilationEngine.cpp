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
        numLocals = 0;
        currentSymbols.clear();
        executeIfLabels = 0;
        executeElseLabels = 0;
        whileLabels = 0;
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
        mySymbolTable.startSubroutine(thisClass);
        //_xmlOutput << "<subroutineDec>" << endl;
        subroutineType = myTokenizer.keyWord();
        //cout << subroutineType << endl;
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
            mySymbolTable.Define("this", thisClass, "ARG");
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("argument"));
            myVMWriter.writePush("ARG", 0);
            myVMWriter.writePop("POINTER", 0);
        }
        else if(subroutineType == "function" || subroutineType == "constructor")
        {
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("argument"));
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
        string parameterType = "";
        string paramID = "";
        while (myTokenizer.symbol() != ')')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                //_xmlOutput << "    <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
                parameterType = myTokenizer.keyWord();
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_xmlOutput << "    <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
                mySymbolTable.Define(myTokenizer.identifier(), parameterType, "ARG");
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
        string currentVarType = "";
        //_xmlOutput << "    <varDec> " << endl;
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl; //var
        myTokenizer.advance();

        if (myTokenizer.tokenType() == "KEYWORD") //var type will either be a class name or a keyword (int, char, bool)
        {
            //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            currentVarType = myTokenizer.keyWord();
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            currentVarType = myTokenizer.identifier();
        }

        myTokenizer.advance();

        mySymbolTable.Define(myTokenizer.identifier(), currentVarType, "VAR");
        myTokenizer.advance();

        while (myTokenizer.symbol() != ';')
        {
            if (myTokenizer.tokenType() == "SYMBOL")
            {
                //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>" << endl; ','
            }
            else if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>" << endl; var name
                mySymbolTable.Define(myTokenizer.identifier(), currentVarType, "VAR");
            }
            myTokenizer.advance();
        }
        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_xmlOutput << "    </varDec> " << endl;
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
        string doIdentifier = "";
        string doSubroutine = "";
        //_xmlOutput << "    <doStatement> " << endl;
        
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl; 'do'
        myTokenizer.advance();

            /*SUBROUTINE CALL*/
            if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                doIdentifier = myTokenizer.identifier();
                myTokenizer.advance();
                
                if(myTokenizer.symbol() == '.') // '.' subRoutine Name '(' expressionList ')'
                {
                    //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                    myTokenizer.advance();

                    doSubroutine = myTokenizer.identifier();
                    //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                    myTokenizer.advance();
                }

                    //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                    myTokenizer.advance();
                    
                    compileExpressionList();

                    //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
                    myTokenizer.advance();
                    myVMWriter.writeCall(doIdentifier + "." + doSubroutine, numLocals);
                    numLocals = 0;
            }

            //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'
            
            //_xmlOutput << "    </doStatement> " << endl;
        return;
    }
    void CompilationEngine::compileLet()
    {
        string varToSet = "";
        //_xmlOutput << "    <letStatement> " << endl;
        
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();
            
        //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
        varToSet = myTokenizer.identifier();
        myTokenizer.advance();

        if (myTokenizer.symbol() == '[')
        {
        //    _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '['
            myTokenizer.advance();

            compileExpression();
            myTokenizer.advance();
            
        //    _xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ']'
            myTokenizer.advance();
        }
        
        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '='
        myTokenizer.advance();

        compileExpression();
        myTokenizer.advance();
        cout << "Kind test: " << mySymbolTable.KindOf(varToSet) << endl;
        cout << "Type test: " << mySymbolTable.TypeOf(varToSet) << endl;
        cout << "Index test: " << mySymbolTable.IndexOf(varToSet) << endl;
        myVMWriter.writePop(mySymbolTable.KindOf(varToSet), mySymbolTable.IndexOf(varToSet));

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        //_xmlOutput << "    </letStatement> " << endl;
        return;
    }
    void CompilationEngine::compileWhile()
    {
        //_xmlOutput << "    <whileStatement> " << endl;
        string currentContinueWhileLabel = "L" + to_string(whileLabels);
        whileLabels++;

        string currentExitWhileLabel = "L" + to_string(whileLabels);
        whileLabels++;

        myVMWriter.writeLabel(currentContinueWhileLabel);
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
        myTokenizer.advance();

        compileExpression();
        myVMWriter.writeArithmetic("not");
        myVMWriter.writeIf(currentExitWhileLabel);

        myTokenizer.advance();
        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'

        myTokenizer.advance();

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'

        compileStatements();

        myVMWriter.writeGoto(currentContinueWhileLabel);

        myVMWriter.writeLabel(currentExitWhileLabel);

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
        
        //_xmlOutput << "    </whileStatement> " << endl;
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
        else
        {
            myVMWriter.writePush("CONST", 0);
        }
        myVMWriter.writeReturn();
        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        //_xmlOutput << "    </returnStatement> " << endl;
        return;
    }
    void CompilationEngine::compileIf()
    {
        string executeIf = "if" + to_string(executeIfLabels);
        executeIfLabels++;
        string executeElse = "else" + to_string(executeElseLabels);
        executeElseLabels++;
        //_xmlOutput << "    <ifStatement> " << endl;
        
        //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
        myTokenizer.advance();

        compileExpression();
        myTokenizer.advance();
        myVMWriter.writeArithmetic("not");
        myVMWriter.writeIf(executeElse);

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
        myTokenizer.advance();

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'
        myTokenizer.advance();

        compileStatements();
        myVMWriter.writeGoto(executeIf);

        //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
        myTokenizer.advance();

        if (myTokenizer.keyWord() == "else")
        {
            myVMWriter.writeLabel(executeElse);
            //_xmlOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
            myTokenizer.advance();
            
            //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'
            myTokenizer.advance();

            compileStatements();

            //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
            myTokenizer.advance();
        }
        myVMWriter.writeLabel(executeIf);
        //_xmlOutput << "    </ifStatement> " << endl;
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
 
            myTokenizer.advance();
            compileTerm();
            if (currentSymbols.back() == "+")
            {
                myVMWriter.writeArithmetic("add");
            }
            else if (currentSymbols.back() == "-")
            {
                myVMWriter.writeArithmetic("sub");
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
                myVMWriter.writeArithmetic("and");
            }
            else if (currentSymbols.back() == "|")
            {
                myVMWriter.writeArithmetic("or");
            }
            else if (currentSymbols.back() == "<")
            {
                myVMWriter.writeArithmetic("lt");
            }
            else if (currentSymbols.back() == ">")
            {
                myVMWriter.writeArithmetic("gt");
            }
            else if (currentSymbols.back() == "=")
            {
                myVMWriter.writeArithmetic("eq");
            }
            currentSymbols.pop_back();
        }
        //_xmlOutput << "      </expression> " << endl;
        return;
    }

    void CompilationEngine::compileTerm()
    {
        string tempToken = "";
        string termIdentifier = "";
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
        else if (myTokenizer.keyWord() == "false" | myTokenizer.keyWord() == "null")
        {
            //_xmlOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
            myVMWriter.writePush("CONST", 0);
        }
        else if (myTokenizer.keyWord() == "true")
        {
            //_xmlOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
            myVMWriter.writePush("CONST", 1);
            myVMWriter.writeArithmetic("neg");
        }
        else if (myTokenizer.keyWord() == "this" )
        {
            //_xmlOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            //_xmlOutput << "      <identifier> " << myTokenizer.identifier() << " </identifier> " << endl;
            termIdentifier = myTokenizer.identifier();

            if (myTokenizer.getNextToken() == "[" | myTokenizer.getNextToken() == "(") //array or variable
            {
                myTokenizer.advance();
                
                //_xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl; // '(' | '['
                myTokenizer.advance();

                compileExpression();

                myTokenizer.advance();
                
                //_xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl; // ')' | ']'
            }
            else if(myTokenizer.getNextToken() == ".") //subroutineCall
            {
                myTokenizer.advance();
                //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                myTokenizer.advance();

                //_xmlOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                currentFunction = myTokenizer.identifier();
                myTokenizer.advance();
            

                //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                myTokenizer.advance();
                
                compileExpressionList();

                //_xmlOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
                myVMWriter.writeCall(termIdentifier + "." + currentFunction, numLocals);
                numLocals = 0;
            }
            else
            {
                myVMWriter.writePush(mySymbolTable.KindOf(termIdentifier), mySymbolTable.IndexOf(termIdentifier));
            }
        }
        else if (myTokenizer.symbol() == '-' || myTokenizer.symbol() == '~') //unaryop term
        {
           // _xmlOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            currentSymbol = myTokenizer.symbol();
            myTokenizer.advance();
            compileTerm();
            if (currentSymbol == "-")
            {
                myVMWriter.writeArithmetic("neg");
            }
            else
            {
                myVMWriter.writeArithmetic("not");
            }
            
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
            numLocals++;
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
                    numLocals++;
                }   
            }
        }
        //_xmlOutput << "      </expressionList>" << endl;
        return;
    }
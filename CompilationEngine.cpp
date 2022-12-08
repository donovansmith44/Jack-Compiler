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
        _vmOutput.open(vmFileName);
        thisClass = "";
        currentFunction = "";
        subroutineType = "";
        currentIdentifier = "";
        currentSymbol = "";
        numArgs = 0;
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
                //_vmOutput << "<class>" << endl;
                //_vmOutput << " <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;

                myTokenizer.advance(); //token should now be className
                thisClass = myTokenizer.keyWord(); //setting thisClass to the current class name for reference in subroutine symbol tables
                //_vmOutput << " <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;


                myTokenizer.advance(); //{
                //_vmOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
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

        //_vmOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_vmOutput << "</class>" << endl;
        _vmOutput.close();
        return;
    }
    void CompilationEngine::compileClassVarDec()
    {
        string classVarType = "";
        string classVarKind = "";
        string classVarName = "";
        //_vmOutput << "<classVarDec>" << endl;
        //_vmOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
        classVarKind = myTokenizer.keyWord(); //kind will be field or static
        myTokenizer.advance();
        
        if (myTokenizer.tokenType() == "KEYWORD") //if the tokenType is a keyword, the type of the static or field variable will be a primitive type (int, boolean, char)
        {
            //_vmOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            classVarType = myTokenizer.keyWord();
            myTokenizer.advance();
        }
        else if(myTokenizer.tokenType() == "IDENTIFIER") //if the tokenType is an identifier, then the type of the static or field variable will be a class type.
        {
            //_vmOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            classVarType = myTokenizer.identifier();
            myTokenizer.advance();
        }
        else
        {

        }

        while(myTokenizer.symbol() != ';')
        {
            if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_vmOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
                classVarName = myTokenizer.identifier();
                mySymbolTable.Define(classVarName, classVarType, classVarKind);
                // cout << "classVarName: " << classVarName << endl;
                // cout << "ClassVarType: " << mySymbolTable.TypeOf(classVarName) << endl;
                // cout << "ClassVarKind: " << mySymbolTable.KindOf(classVarName) << endl;
                // cout << "ClassVarIndex: " << mySymbolTable.IndexOf(classVarName) << endl;
                myTokenizer.advance();
            }
            else
            {
                /*an error should be thrown, because we expected a variable name of type identifier*/
            }   
            if(myTokenizer.symbol() == ',') //if we encounter a comma, that means that we should expect more variable names.
            {
                //_vmOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
                myTokenizer.advance();
                continue;
            }
            else if(myTokenizer.symbol() == ';')
            {
                break;
            }
        }
        //_vmOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_vmOutput << " </classVarDec>" << endl;
        return;
    }
    void CompilationEngine::compileSubroutine()
    {
        mySymbolTable.startSubroutine(thisClass);
        //_vmOutput << "<subroutineDec>" << endl;
        subroutineType = myTokenizer.keyWord();
        //_vmOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl; //constructor, function, or method
        myTokenizer.advance();

        if(myTokenizer.tokenType() == "IDENTIFIER") //the function/constructor will have a return type of 'identifier' 
        {
            //_vmOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            myTokenizer.advance();
        }
        else if (myTokenizer.tokenType() == "KEYWORD") //the function/constructor will have a predefined return type (int, void, char...)
        {
            //_vmOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            myTokenizer.advance();
        }
        else
        {
            /*this shouldn't be possible*/
        }

        //_vmOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl; //name of the function
        currentFunction = myTokenizer.identifier();

        myTokenizer.advance();

        //_vmOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl; //(
        myTokenizer.advance();

        compileParameterList();

        //_vmOutput << "  <symbol> " << myTokenizer.symbol() << " </symbol>" << endl; //)


        /*compile subroutine body*/
        //_vmOutput << "  <subroutineBody>" << endl;
        myTokenizer.advance();
        //_vmOutput << "   <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        
        myTokenizer.advance();

        while (myTokenizer.keyWord() == "var")
        {
            compileVarDec();
            myTokenizer.advance();
        }

        if (subroutineType == "method")
        {
            mySymbolTable.Define("this", thisClass, "ARG");
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("local"));
            myVMWriter.writePush("argument", 0);
            myVMWriter.writePop("POINTER", 0);
        }
        else if(subroutineType == "function" || subroutineType == "constructor")
        {
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("local"));
        }
        else
        {
            /*this should not be possible.*/            
        }

        compileStatements();

        //_vmOutput << "   <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_vmOutput << "  </subroutineBody>" << endl;
        //_vmOutput << " </subroutineDec>" << endl;
        return;
    }
    void CompilationEngine::compileParameterList()
    {
        //_vmOutput << "   <parameterList> " << endl;
        string parameterType = "";
        string paramID = "";
        while (myTokenizer.symbol() != ')')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                //_vmOutput << "    <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
                parameterType = myTokenizer.keyWord();
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_vmOutput << "    <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
                mySymbolTable.Define(myTokenizer.identifier(), parameterType, "ARG");
            }
            else if(myTokenizer.symbol() == ',')
            {
                //_vmOutput << "    <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
            }
            myTokenizer.advance();
        }
        //_vmOutput << "   </parameterList> "<< endl;
        return;
    }
    void CompilationEngine::compileVarDec()
    {
        string currentVarType = "";
        //_vmOutput << "    <varDec> " << endl;
        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl; //var
        myTokenizer.advance();

        if (myTokenizer.tokenType() == "KEYWORD") //var type will either be a class name or a keyword (int, char, bool)
        {
            //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            currentVarType = myTokenizer.keyWord();
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            //_vmOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            currentVarType = myTokenizer.identifier();
        }

        myTokenizer.advance();

        mySymbolTable.Define(myTokenizer.identifier(), currentVarType, "VAR");
        myTokenizer.advance();

        while (myTokenizer.symbol() != ';')
        {
            if (myTokenizer.tokenType() == "SYMBOL")
            {
                //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>" << endl; ','
            }
            else if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_vmOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>" << endl; var name
                mySymbolTable.Define(myTokenizer.identifier(), currentVarType, "VAR");
            }
            myTokenizer.advance();
        }
        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //_vmOutput << "    </varDec> " << endl;
        return;
    }
    void CompilationEngine::compileStatements()
    {
        //_vmOutput << "   <statements> " << endl;
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
      //  _vmOutput << "   </statements> " << endl;
        return;
    }
    void CompilationEngine::compileDo() 
    {
        string doIdentifier = "";
        string doSubroutine = "";
        //_vmOutput << "    <doStatement> " << endl;
        
        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl; 'do'
        myTokenizer.advance();

            /*SUBROUTINE CALL*/
            if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_vmOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                doIdentifier = myTokenizer.identifier();
                myTokenizer.advance();
                
                if(myTokenizer.symbol() == '.') // '.' subRoutine Name '(' expressionList ')'
                {
                    //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                    myTokenizer.advance();

                    doSubroutine = myTokenizer.identifier();
                    //_vmOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                    myTokenizer.advance();
                }

                    //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                    myTokenizer.advance();
                    
                    compileExpressionList();

                    //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
                    myTokenizer.advance();
                    myVMWriter.writeCall(doIdentifier + "." + doSubroutine, numArgs);
                    myVMWriter.writePop("TEMP", 0);
                    numArgs = 0;
            }

            //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'
            
            //_vmOutput << "    </doStatement> " << endl;
        return;
    }
    void CompilationEngine::compileLet()
    {
        string varToSet = "";
        //_vmOutput << "    <letStatement> " << endl;
        
        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();
            
        //_vmOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
        varToSet = myTokenizer.identifier();
        myTokenizer.advance();

        if (myTokenizer.symbol() == '[')
        {
        //    _vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '['
            myTokenizer.advance();

            compileExpression();
            myTokenizer.advance();
            
        //    _vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ']'
            myTokenizer.advance();
        }
        
        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '='
        myTokenizer.advance();

        compileExpression();
        myTokenizer.advance();

        myVMWriter.writePop(mySymbolTable.KindOf(varToSet), mySymbolTable.IndexOf(varToSet));

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        //_vmOutput << "    </letStatement> " << endl;
        return;
    }
    void CompilationEngine::compileWhile()
    {
        //_vmOutput << "    <whileStatement> " << endl;
        string currentContinueWhileLabel = "L" + to_string(whileLabels);
        whileLabels++;

        string currentExitWhileLabel = "L" + to_string(whileLabels);
        whileLabels++;

        myVMWriter.writeLabel(currentContinueWhileLabel);
        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
        myTokenizer.advance();

        compileExpression();
        myVMWriter.writeArithmetic("not");
        myVMWriter.writeIf(currentExitWhileLabel);

        myTokenizer.advance();
        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'

        myTokenizer.advance();

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'

        compileStatements();

        myVMWriter.writeGoto(currentContinueWhileLabel);

        myVMWriter.writeLabel(currentExitWhileLabel);

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
        
        //_vmOutput << "    </whileStatement> " << endl;
        return;
    }
    void CompilationEngine::compileReturn()
    {
        //_vmOutput << "    <returnStatement> " << endl;
        
        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
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
        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ';'

        //_vmOutput << "    </returnStatement> " << endl;
        return;
    }
    void CompilationEngine::compileIf()
    {
        string executeIf = "if" + to_string(executeIfLabels);
        executeIfLabels++;
        string executeElse = "else" + to_string(executeElseLabels);
        executeElseLabels++;
        //_vmOutput << "    <ifStatement> " << endl;
        
        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        myTokenizer.advance();

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
        myTokenizer.advance();

        compileExpression();
        myTokenizer.advance();
        myVMWriter.writeArithmetic("not");
        myVMWriter.writeIf(executeElse);

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
        myTokenizer.advance();

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'
        myTokenizer.advance();

        compileStatements();
        myVMWriter.writeGoto(executeIf);

        //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
        myTokenizer.advance();

        if (myTokenizer.keyWord() == "else")
        {
            myVMWriter.writeLabel(executeElse);
            //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
            myTokenizer.advance();
            
            //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '{'
            myTokenizer.advance();

            compileStatements();

            //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '}'
            myTokenizer.advance();
        }
        myVMWriter.writeLabel(executeIf);
        //_vmOutput << "    </ifStatement> " << endl;
        return;
    }
    void CompilationEngine::compileExpression()
    {
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
        return;
    }

    void CompilationEngine::compileTerm()
    {
        string tempToken = "";
        string termIdentifier = "";

        if (myTokenizer.tokenType() == "INT_CONST")
        {
          //  _vmOutput << "      <integerConstant> " << myTokenizer.intVal() << " </integerConstant> " << endl;
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
            _vmOutput << "      <stringConstant> " << tempToken << " </stringConstant> " << endl;
        }
        else if (myTokenizer.keyWord() == "false" | myTokenizer.keyWord() == "null")
        {
            //_vmOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
            myVMWriter.writePush("CONST", 0);
        }
        else if (myTokenizer.keyWord() == "true")
        {
            //_vmOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
            myVMWriter.writePush("CONST", 1);
            myVMWriter.writeArithmetic("neg");
        }
        else if (myTokenizer.keyWord() == "this" )
        {
            //_vmOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
            myVMWriter.writePush("THIS", 0);
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            //_vmOutput << "      <identifier> " << myTokenizer.identifier() << " </identifier> " << endl;
            termIdentifier = myTokenizer.identifier();

            if (myTokenizer.getNextToken() == "[" | myTokenizer.getNextToken() == "(") //array or variable
            {
                myTokenizer.advance();
                
                //_vmOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl; // '(' | '['
                myTokenizer.advance();

                compileExpression();

                myTokenizer.advance();
                
                //_vmOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl; // ')' | ']'
            }
            else if(myTokenizer.getNextToken() == ".") //subroutineCall
            {
                myTokenizer.advance();
                //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '.'
                myTokenizer.advance();

                //_vmOutput << "     <identifier> " << myTokenizer.identifier() << " </identifier>"  << endl; //subroutineName
                currentFunction = myTokenizer.identifier();
                myTokenizer.advance();
            

                //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // '('
                myTokenizer.advance();
                
                compileExpressionList();

                //_vmOutput << "     <symbol> " << myTokenizer.symbol() << " </symbol>"  << endl; // ')'
                myVMWriter.writeCall(termIdentifier + "." + currentFunction, numArgs);
                numArgs = 0;
            }
            else
            {
                myVMWriter.writePush(mySymbolTable.KindOf(termIdentifier), mySymbolTable.IndexOf(termIdentifier));
            }
        }
        else if (myTokenizer.symbol() == '-' || myTokenizer.symbol() == '~') //unaryop term
        {
           // _vmOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            string currentOp = ""; 

            currentOp = myTokenizer.symbol();
            myTokenizer.advance();

            compileTerm();
            if (currentOp == "-")
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
         //   _vmOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
            myTokenizer.advance();
            compileExpression();
            myTokenizer.advance();
           // _vmOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
        }        
        
        return;
    }
    void CompilationEngine::compileExpressionList()
    {
        if (myTokenizer.symbol() != ')')
        {
            numArgs++;
            compileExpression();
            myTokenizer.advance();
           
           while (myTokenizer.symbol() != ')')
            {
                if (myTokenizer.symbol() == ',')
                {
          //          _vmOutput << "      <symbol> " << myTokenizer.symbol() << " </symbol> " << endl;
                    myTokenizer.advance();
                    compileExpression();
                    myTokenizer.advance();
                    numArgs++;
                }   
            }
        }
        return;
    }
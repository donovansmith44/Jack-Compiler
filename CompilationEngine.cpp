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
        //if (myTokenizer.hasMoreTokens()) //class declaration
        //{
                myTokenizer.advance(); //the first token in the tokenizer should be 'class'
                //_vmOutput << "<class>" << endl;
                //_vmOutput << " <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;

                myTokenizer.advance(); //token should now be className
                thisClass = myTokenizer.keyWord(); //setting thisClass to the current class name for reference in subroutine symbol tables
                //_vmOutput << " <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;


                myTokenizer.advance(); //{
                //_vmOutput << " <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
        //}

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
                else
                {
                    /*We have reached the end of the jack file.*/
                }
            }            
        }

        _vmOutput.close();
        return;
    }
    void CompilationEngine::compileClassVarDec()
    {
        string classVarType = "";
        string classVarKind = "";
        string classVarName = "";

        classVarKind = myTokenizer.keyWord(); //kind will be field or static
        myTokenizer.advance();
        
        if (myTokenizer.tokenType() == "KEYWORD") //if the tokenType is a keyword, the type of the static or field variable will be a primitive type (int, boolean, char)
        {
            classVarType = myTokenizer.keyWord();
            myTokenizer.advance();
        }
        else if(myTokenizer.tokenType() == "IDENTIFIER") //if the tokenType is an identifier, then the type of the static or field variable will be a class type.
        {
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
                classVarName = myTokenizer.identifier();
                mySymbolTable.Define(classVarName, classVarType, classVarKind);
                myTokenizer.advance();
            }
            else
            {
                /*an error should be thrown, because we expected a variable name of type identifier*/
            }

            if(myTokenizer.symbol() == ',') //if we encounter a comma, that means that we should expect more variable names.
            {
                myTokenizer.advance();
                continue;
            }
            else if(myTokenizer.symbol() == ';')
            {
                break;
            }
            else
            {

            }
        }

        return;
    }
    void CompilationEngine::compileSubroutine()
    {
        mySymbolTable.startSubroutine(thisClass);

        subroutineType = myTokenizer.keyWord(); //subroutineType is constructor, function, or method
         
        myTokenizer.advance();

        if(myTokenizer.tokenType() == "IDENTIFIER") //the function/constructor will have a return type of 'identifier' 
        {
            //_vmOutput << "  <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
            currentSubroutineReturnType = myTokenizer.identifier();
            myTokenizer.advance();
        }
        else if (myTokenizer.tokenType() == "KEYWORD") //the function/constructor will have a predefined return type (int, void, char...)
        {
            //_vmOutput << "  <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
            currentSubroutineReturnType = myTokenizer.keyWord();
            myTokenizer.advance();
        }
        else
        {
            /*this shouldn't be possible*/
        }
        
        /* 'functionName' '(' 'parameterList' ')' */
        currentFunction = myTokenizer.identifier(); //name of the function

        myTokenizer.advance(); // '('

        myTokenizer.advance(); // ')' or some parameter is the current token here. if it's a ')', compileParameterList does not advance the tokenizer past that token.

        compileParameterList();

        myTokenizer.advance(); // ')'

        /*compile subroutine body*/

        myTokenizer.advance(); // '{'
        
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
        else if(subroutineType == "function")
        {
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("local"));
        }
        else if(subroutineType == "constructor")
        {
            myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("local"));
        }
        else
        {
            /*this should not be possible.*/            
        }

        compileStatements();

        return;
    }
    void CompilationEngine::compileParameterList()
    {
        string parameterType = "";
        string paramID = "";

        while (myTokenizer.symbol() != ')')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                //_vmOutput << "    <keyword> " << myTokenizer.keyWord() << " </keyword>" << endl;
                parameterType = myTokenizer.keyWord();
                //cout << "Keyword: " << myTokenizer.keyWord() << endl;
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                //_vmOutput << "    <identifier> " << myTokenizer.identifier() << " </identifier>" << endl;
                mySymbolTable.Define(myTokenizer.identifier(), parameterType, "ARG");
                //cout << "Identifier: " << myTokenizer.identifier() << endl;
            }
            else if(myTokenizer.symbol() == ',')
            {
                //_vmOutput << "    <symbol> " << myTokenizer.symbol() << " </symbol>" << endl;
                //cout << "Symbol: " << myTokenizer.symbol() << endl;
            }
            myTokenizer.advance();
        }
        return;
    }
    void CompilationEngine::compileVarDec()
    {
        string currentVarType = "";

        myTokenizer.advance(); // var

        if (myTokenizer.tokenType() == "KEYWORD") //var type will either be a class name or a keyword (int, char, bool)
        {
            currentVarType = myTokenizer.keyWord();
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
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
        string subroutineCaller = "";
        string subroutineCallerType = "";
        string subroutine = "";

        myTokenizer.advance();

        if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            subroutineCaller = myTokenizer.identifier();

            if (mySymbolTable.TypeOf(subroutineCaller) != "NONE") //if the type of the subroutineCaller is not "NONE", then that means that the subroutineCaller is an object, with a type corresponding to its respective class.
            {
                subroutineCallerType = mySymbolTable.TypeOf(subroutineCaller);
                myVMWriter.writePush(mySymbolTable.KindOf(subroutineCaller), mySymbolTable.VarCount(subroutineCaller)); //the object being operated on is a caller argument, and must be pushed onto the stack
                numArgs++;
            }
            else //this condition will handle the types of subroutineCallers that are predefined by the OS
            {
                subroutineCallerType = myTokenizer.identifier();
            }

            myTokenizer.advance();
            
            if(myTokenizer.symbol() == '.') // '.' subRoutine Name
            {
                myTokenizer.advance();

                subroutine = myTokenizer.identifier();
                
                myTokenizer.advance();
            }
            else
            {
                /*This should not be possible. do statements always have a caller.*/
            }
                
            myTokenizer.advance(); // '('
            
            compileExpressionList();

            myTokenizer.advance(); // ')'

            myVMWriter.writeCall(subroutineCallerType + "." + subroutine, numArgs);
            myVMWriter.writePop("TEMP", 0);
            numArgs = 0; //all arguments pushed onto the stack should be eaten up by the called function, therefore we reset the number of arguments to 0 after every function call.
        }
        else
        {
            /*This should not be possible. We expect an identifier (which is the object that calls the subroutine) immediately following 'do' in our tokenizer.*/
        }

        return;
    }
    void CompilationEngine::compileLet()
    {
        string varToSet = "";

        //_vmOutput << "     <keyword> " << myTokenizer.keyWord() << " </keyword>"  << endl;
        //cout << myTokenizer.keyWord() << endl;
        myTokenizer.advance();

        //cout << myTokenizer.identifier() << endl;
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
        string termIdentifier = "";

        if (myTokenizer.tokenType() == "INT_CONST")
        {
            myVMWriter.writePush("CONST", myTokenizer.intVal());
        }
        else if (myTokenizer.tokenType() == "STRING_CONST")
        {
            string stringConstant = myTokenizer.stringVal();

            if(stringConstant.find('"') != string::npos) //string constants are identified with opening and closing '"' during tokenization. Remove them for compilation.
            {
                stringConstant.erase(stringConstant.find('"'));
            }
            else
            {

            }

            if(stringConstant.find('"') != string::npos)
            {
                stringConstant.erase(stringConstant.find('"'));
            }
            else
            {

            }
        }
        else if (myTokenizer.keyWord() == "false" | myTokenizer.keyWord() == "null")
        {
            myVMWriter.writePush("CONST", 0);
        }
        else if (myTokenizer.keyWord() == "true")
        {
            myVMWriter.writePush("CONST", 1);
            myVMWriter.writeArithmetic("neg");
        }
        else if (myTokenizer.keyWord() == "this" )
        {
            //_vmOutput << "      <keyword> " << myTokenizer.keyWord() << " </keyword> " << endl;
            //do we assume that pointer is already set?
            //YES, because when we declare a method, this is defined as argument 0 in our symbol table, and 
            myVMWriter.writePush("POINTER", 0);
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            termIdentifier = myTokenizer.identifier();

            if (myTokenizer.getNextToken() == "[" | myTokenizer.getNextToken() == "(") //array or variable
            {
                myTokenizer.advance();
                
                myTokenizer.advance(); // '(' or '['

                compileExpression();

                myTokenizer.advance(); // ')' or ']'
            }
            else if(myTokenizer.getNextToken() == ".") //subroutineCall
            {
                string subroutine = "";
                myTokenizer.advance(); // '.'

                myTokenizer.advance(); // subroutineName

                subroutine = myTokenizer.identifier();

                myTokenizer.advance();            

                myTokenizer.advance(); // '('
                
                compileExpressionList();
                                       // ')'

                myVMWriter.writeCall(termIdentifier + "." + subroutine, numArgs);
                numArgs = 0;
            }
            else
            {
                myVMWriter.writePush(mySymbolTable.KindOf(termIdentifier), mySymbolTable.IndexOf(termIdentifier));
            }
        }
        else if (myTokenizer.symbol() == '-' || myTokenizer.symbol() == '~') //unaryop term
        {
            string currentUnaryOp = ""; 

            currentUnaryOp = myTokenizer.symbol();
            myTokenizer.advance();

            compileTerm();
            if (currentUnaryOp == "-")
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
            myTokenizer.advance(); // '('
            compileExpression();
            myTokenizer.advance(); // ')'
        }
        else
        {

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
                    myTokenizer.advance();
                    compileExpression();
                    myTokenizer.advance();
                    numArgs++;
                }   
                else
                {

                }
            }
        }
        return;
    }
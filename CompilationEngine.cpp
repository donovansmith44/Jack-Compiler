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

        numArgs = 0;
        ifLabels = 0;
        whileLabels = 0;
        thisClass = "";
        currentFunction = "";
        currentSymbols.clear();
    }
    void CompilationEngine::compileClass()
    {
        myTokenizer.advance(); //the first token in the tokenizer should be 'class'

        myTokenizer.advance(); //token should now be className
        thisClass = myTokenizer.keyWord(); //setting thisClass to the current class name for reference in subroutine symbol tables

        myTokenizer.advance(); //{

        while (myTokenizer.hasMoreTokens())
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
            else
            {
                /*expected a keyword*/
            }
        }
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
        else if(myTokenizer.tokenType() == "IDENTIFIER") //if the tokenType is an identifier, then the type of the static or field variable will be a class type (an object)
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
        }

        return;
    }
    void CompilationEngine::compileSubroutine()
    {
        string subroutineType = "";

        mySymbolTable.startSubroutine();

        subroutineType = myTokenizer.keyWord(); //subroutineType is constructor, function, or method

        if (subroutineType == "method")
        {
            mySymbolTable.Define("this", thisClass, "ARG"); //methods require an argument called "this" to be stored in their symbol table, to point to the current object
        }

        myTokenizer.advance();

        if(myTokenizer.tokenType() == "IDENTIFIER") //the function/constructor will have a return type of 'identifier' 
        {
            currentSubroutineReturnType = myTokenizer.identifier();
        }
        else if (myTokenizer.tokenType() == "KEYWORD") //the function/constructor will have a predefined return type (int, void, char...)
        {
            currentSubroutineReturnType = myTokenizer.keyWord();
        }
        
        myTokenizer.advance();

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
        
        myVMWriter.writeFunction(thisClass + "." + currentFunction, mySymbolTable.VarCount("local"));

        if (subroutineType == "method")
        {    
            myVMWriter.writePush("argument", 0);
            myVMWriter.writePop("POINTER", 0);
        }
        else if(subroutineType == "constructor")
        {
            myVMWriter.writePush("CONST", mySymbolTable.VarCount("field"));
            myVMWriter.writeCall("Memory.alloc", 1);
            myVMWriter.writePop("POINTER", 0);
        }

        compileStatements();

        return;
    }
    void CompilationEngine::compileParameterList()
    {
        string parameterType = "";
        string parameterID = "";

        while (myTokenizer.symbol() != ')')
        {
            if (myTokenizer.tokenType() == "KEYWORD")
            {
                parameterType = myTokenizer.keyWord();
            }
            else if(myTokenizer.tokenType() == "IDENTIFIER")
            {
                parameterType = myTokenizer.identifier();
            }
            else
            {
                /*This should not be possible. If this loop is entered, there must be 
                parameters present which will have a type of "KEYWORD" or "IDENTIFIER"*/
            }

            myTokenizer.advance();

            parameterID = myTokenizer.identifier();

            mySymbolTable.Define(parameterID, parameterType, "ARG");
            
            myTokenizer.advance();

            if(myTokenizer.symbol() == ',')
            { 
                myTokenizer.advance();
            }
            else
            {
                /*We have reached the end of the parameter list.*/
            }
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

        while (myTokenizer.symbol() != ';')
        {
           if (myTokenizer.tokenType() == "IDENTIFIER")
            {
                mySymbolTable.Define(myTokenizer.identifier(), currentVarType, "VAR");
            }
            else
            {
                /*There will be commas in the case that there are multiple variables declared at once*/
            }
            myTokenizer.advance();
        }
        return;
    }
    void CompilationEngine::compileStatements()
    {
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
                myTokenizer.advance();
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
        return;
    }
    void CompilationEngine::compileDo() 
    {
        /*
        'do' 'function-or-method-call' ';'
        */
        string subroutineCaller = "";
        string subroutineCallerType = "";
        string subroutine = "";

        myTokenizer.advance(); // 'do'

        if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            subroutineCaller = myTokenizer.identifier();

            if (mySymbolTable.TypeOf(subroutineCaller) != "NONE") //if the type of the subroutineCaller is not "NONE", then that means that the subroutineCaller is a programmer-defined object, with a type corresponding to its respective class.
            {
                subroutineCallerType = mySymbolTable.TypeOf(subroutineCaller);

                if (mySymbolTable.KindOf(subroutineCaller) == "field")
                {
                    myVMWriter.writePush("THIS", mySymbolTable.IndexOf(subroutineCaller)); //the object being operated on is a caller argument, and must be pushed onto the stack
                }
                else
                {
                    myVMWriter.writePush(mySymbolTable.KindOf(subroutineCaller), mySymbolTable.VarCount(subroutineCaller)); //the object being operated on is a caller argument, and must be pushed onto the stack
                }
                numArgs++;
            }
            else //if the type of the subroutineCaller is "NONE", then the subroutineCallerType is predefined by the OS.
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
                /*This should not be possible. do statements always have a caller which is followed by '.'*/
            }
                
            myTokenizer.advance(); // '('
                        
            if (subroutine != "")
            {   compileExpressionList();
                myTokenizer.advance(); // ')'
                myVMWriter.writeCall(subroutineCallerType + "." + subroutine, numArgs);
            }
            else
            {
                myVMWriter.writePush("POINTER", 0);
                compileExpressionList();
                myTokenizer.advance(); // ')'
                numArgs++;
                myVMWriter.writeCall(thisClass + "." + subroutineCallerType, numArgs);
            }

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
        /*
        'let' 'variable' '=' expression ';' 
        or
        'let' 'variable' '[' expression ']' '=' expression ';' 
        */
        bool accessingArray = false;
        string varToSet = "";

        myTokenizer.advance();

        varToSet = myTokenizer.identifier();

        myTokenizer.advance();

        if (myTokenizer.symbol() == '[')
        {
            accessingArray = true;
            myVMWriter.writePush(mySymbolTable.KindOf(varToSet), mySymbolTable.IndexOf(varToSet));

            myTokenizer.advance();

            compileExpression();

            myTokenizer.advance();
            
            myTokenizer.advance(); // ']'
            myVMWriter.writeArithmetic("ADD");
        }
                
        myTokenizer.advance(); // '='

        compileExpression();

        myTokenizer.advance();

        if (mySymbolTable.KindOf(varToSet) == "field")
        {
            myVMWriter.writePop("THIS", mySymbolTable.IndexOf(varToSet));
        }
        else if(accessingArray)
        {
            myVMWriter.writePop("TEMP", 0);
            myVMWriter.writePop("POINTER", 1);
            myVMWriter.writePush("TEMP", 0);
            myVMWriter.writePop("THAT", 0);
        }
        else
        {
            myVMWriter.writePop(mySymbolTable.KindOf(varToSet), mySymbolTable.IndexOf(varToSet));
        }

        return;
    }
    void CompilationEngine::compileWhile()
    {
        /*
        'while' '(' expression ')'
        '{'
            statements
        '}'
        */
        string WHILE_EXP = "WHILE_EXP" + to_string(whileLabels);
        string WHILE_END = "WHILE_END" + to_string(whileLabels);
        whileLabels++;

        myVMWriter.writeLabel(WHILE_EXP);

        myTokenizer.advance();

        myTokenizer.advance(); // '('

        compileExpression();

        myVMWriter.writeArithmetic("not");
        
        myVMWriter.writeIf(WHILE_END);

        myTokenizer.advance(); // ')'

        myTokenizer.advance(); // '{'

        compileStatements();

        myVMWriter.writeGoto(WHILE_EXP);

        myVMWriter.writeLabel(WHILE_END);

        // '}'
        return;
    }
    void CompilationEngine::compileReturn()
    {
        /*
        'return' '(' expression ')' ';' 
        */
        myTokenizer.advance();

        if (myTokenizer.symbol() == ';')
        {
            myVMWriter.writePush("CONST", 0);
            myVMWriter.writeReturn();
            return;
        }

        compileExpression();
        myTokenizer.advance();    

        myVMWriter.writeReturn();
        return;
    }
    void CompilationEngine::compileIf()
    {
        /*
        'if' '(' expression ')' '{' statements '}' 
        */
        string IF_TRUE = "IF_TRUE" + to_string(ifLabels);
        string IF_FALSE = "IF_FALSE" + to_string(ifLabels);
        ifLabels++;

        myTokenizer.advance();

        myTokenizer.advance(); // '('

        compileExpression();

        myTokenizer.advance();

        myVMWriter.writeArithmetic("not");

        myVMWriter.writeIf(IF_FALSE);

        myTokenizer.advance(); // ')'
        
        myTokenizer.advance(); // '{'

        compileStatements();
        myVMWriter.writeGoto(IF_TRUE);

        myVMWriter.writeLabel(IF_FALSE);
        
        /*
        'else' '{' statements '}'
        */
        if (myTokenizer.getNextToken() == "else")
        {
            myTokenizer.advance();  // '}'

            myTokenizer.advance();
            
            myTokenizer.advance(); // '{'

            compileStatements();

            //myTokenizer.advance(); // '}'
        }

        myVMWriter.writeLabel(IF_TRUE);

        return;
    }
    void CompilationEngine::compileExpression()
    {
        /*
        'term'
        or 
        'term' 'operator' 'term' 
        */
        compileTerm();
        
        if (myTokenizer.getNextToken() == "+" | myTokenizer.getNextToken() == "-" | myTokenizer.getNextToken() == "*" | myTokenizer.getNextToken() == "/" | myTokenizer.getNextToken() == "&" | myTokenizer.getNextToken() == "|" | myTokenizer.getNextToken() == "<" | myTokenizer.getNextToken() == ">" | myTokenizer.getNextToken() == "=")
        {
            currentSymbols.push_back(myTokenizer.getNextToken()); //currentSymbols is a class-level vector 

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
            myVMWriter.writePush("CONST", myTokenizer.stringVal().length());
            myVMWriter.writeCall("String.new", 1);

            for (int i = 0; i < myTokenizer.stringVal().length(); i++)
            {
                myVMWriter.writePush("CONST", int(myTokenizer.stringVal()[i]));
                myVMWriter.writeCall("String.appendChar", 2);
            }
        }
        else if (myTokenizer.keyWord() == "false" | myTokenizer.keyWord() == "null")
        {
            myVMWriter.writePush("CONST", 0);
        }
        else if (myTokenizer.keyWord() == "true") //"true" is represented as -1 at the assembly level.
        {
            myVMWriter.writePush("CONST", 1);
            myVMWriter.writeArithmetic("neg");
        }
        else if (myTokenizer.keyWord() == "this" )
        {
            myVMWriter.writePush("POINTER", 0);
        }
        else if (myTokenizer.tokenType() == "IDENTIFIER")
        {
            termIdentifier = myTokenizer.identifier();

            if (myTokenizer.getNextToken() == "[") //array
            {
                myTokenizer.advance();
                
                myTokenizer.advance(); // '['

                myVMWriter.writePush(mySymbolTable.KindOf(termIdentifier), mySymbolTable.IndexOf(termIdentifier));

                compileExpression();

                myVMWriter.writeArithmetic("ADD");
                myVMWriter.writePop("POINTER", 1);
                myVMWriter.writePush("THAT", 0);

                myTokenizer.advance(); // ']'
            }
            else if (myTokenizer.getNextToken() == "(") //variable
            {
                myTokenizer.advance();
                
                myTokenizer.advance(); // '('

                compileExpression();

                myTokenizer.advance(); // ')'
            }
            else if(myTokenizer.getNextToken() == ".") //subroutineCall
            {
                
                string subroutine = "";
                string subroutineType = "";
                subroutineType = myTokenizer.identifier();

                myTokenizer.advance(); // '.'
                
                myTokenizer.advance(); // subroutineName

                subroutine = myTokenizer.identifier();
                
                myTokenizer.advance();            

                myTokenizer.advance(); // '('
                
                compileExpressionList();

                if (mySymbolTable.TypeOf(subroutineType) == "NONE")
                {
                    myVMWriter.writeCall(subroutineType + "." + subroutine, numArgs);
                }
                else
                {
                    myVMWriter.writePush("THIS", mySymbolTable.IndexOf(subroutineType));
                    numArgs++;
                    myVMWriter.writeCall(mySymbolTable.TypeOf(subroutineType) + "." + subroutine, numArgs);
                }

                numArgs = 0;
            }
            else
            {
                if (mySymbolTable.KindOf(termIdentifier) == "field")
                {
                    myVMWriter.writePush("THIS", mySymbolTable.IndexOf(termIdentifier));
                }
                else
                {
                    myVMWriter.writePush(mySymbolTable.KindOf(termIdentifier), mySymbolTable.IndexOf(termIdentifier));
                }
            }
        }
        else if (myTokenizer.symbol() == '-' || myTokenizer.symbol() == '~') //unaryop term (negations and nots)
        {
            char currentUnaryOp = myTokenizer.symbol();
            
            myTokenizer.advance();

            compileTerm();

            switch (currentUnaryOp)
            {
            case '-':
                myVMWriter.writeArithmetic("neg");
                break;
            default:
                myVMWriter.writeArithmetic("not");
                break;
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
        if (myTokenizer.symbol() == ')')
        {
            return;
        }

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
        }
        return;
    }
# Jack-Compiler

The purpose of this compiler is to translate programs written in the Jack programming language
into low-level byte code, resembling that of the JVM.
 
To compile:
g++ -std=c++17 CompilationEngine.cpp VMWriter.cpp JackTokenizer.cpp JackAnalyzer.cpp SymbolTable.cpp

To run:
./a.out "type_directory_containing_jack_files_here" ("Seven", "ConvertToBin", "Square", "Average", "Pong", and "ComplexArrays" included)

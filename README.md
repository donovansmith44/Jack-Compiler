# Jack-Compiler

The purpose of this compiler is to translate programs written in the Jack programming language
into low-level byte code, resembling that of the JVM.

Please note that this compiler is not currently equipped with error-handling mechanisms,
and assumes that programs are error-free.

Array, Memory, Math, Keyboard, Screen, String, and Sys with ".vm" extensions are classes
provided by the Jack OS, and make up the Jack standard library.
 
To compile:
g++ -std=c++17 CompilationEngine.cpp VMWriter.cpp JackTokenizer.cpp JackAnalyzer.cpp SymbolTable.cpp

To run:
./a.out "type_directory_containing_jack_files_here" ("Seven", "ConvertToBin", "Square", "Average", "Pong", and "ComplexArrays" included)

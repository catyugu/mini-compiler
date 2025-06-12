Mini C++-like Compiler
This project is an educational, end-to-end compiler built from scratch in C++. It takes a simple C-style source language and compiles it down to a native, executable x86-64 Linux program. The primary goal of this project is to serve as a practical learning tool for understanding the core principles of compiler design.

Features
The custom language currently supports:

Variable Declarations: Using the let keyword (e.g., let x = ...;).

Data Types: int and float literals are recognized and handled.

Arithmetic Expressions: +, -, *, / with correct operator precedence and associativity.

Grouped Expressions: Using parentheses ().

Type Casting: Explicit casting between types (e.g., (int)my_float;).

External Function Calls: Ability to call pre-compiled C functions.

Compiler Architecture
This compiler is built using a classic multi-stage pipeline, with each stage having a distinct responsibility.

Lexical Analysis (Lexer)

Scans the raw source code string and converts it into a stream of meaningful tokens (keywords, identifiers, numbers, operators).

Syntactic Analysis (Parser)

Takes the token stream and constructs an Abstract Syntax Tree (AST). The AST is a hierarchical representation of the code's structure, respecting grammar rules and operator precedence. This stage is implemented using a recursive descent parser.

Semantic Analysis (Type Checker)

Walks the AST to perform logical checks. Its primary job is type checking—ensuring that operations are performed on compatible data types. It annotates each expression node in the AST with its resulting type (int or float).

Intermediate Representation (IR) Generation

Traverses the type-annotated AST and flattens it into a linear, low-level Intermediate Representation. This project uses a simple Three-Address Code (TAC) format, which makes the final translation to assembly much easier.

Code Generation (Back-End)

The final stage translates the IR into x86-64 assembly code using the NASM syntax. It manages memory for variables on the stack and uses CPU registers for temporary calculations.

Target Platform
Architecture: x86-64

Operating System: Linux

Assembler Syntax: NASM

Calling Convention: System V ABI (for external C function calls)

Dependencies
To build and run this project, you will need:

g++ (version supporting C++17 or later)

nasm (The Netwide Assembler)

ld (The GNU Linker)

gcc (to compile the C runtime file)

How to Build and Run
Follow these steps from the project's root directory.

1. Build the Compiler (mcc)
First, compile the C++ source code of the compiler itself.

g++ src/*.cpp -o mcc -std=c++17

2. Prepare the C Runtime
Our language can call external C functions. We need to compile this C code into an object file.

gcc -c runtime.c -o runtime.o

3. Run the Compiler
Execute your newly built compiler. It will read the source code from main.cpp, process it, and generate an assembly file named output.s.

./mcc

4. Assemble and Link the Generated Code
Now, take the output.s file and turn it into a final executable program, linking it with our C runtime.

# Assemble the generated code
nasm -f elf64 output.s -o output.o

# Link it with the C runtime to create the final program
ld output.o runtime.o -o final_program

5. Run the Final Program!
Execute the program you just created. We can check its result by printing its exit code.

# Run the compiled program
./final_program

# Print the exit code of the last command (this is our result)
echo $?

For the example source let result = (int)my_func(10, 20.5);, the program should correctly output 30.

Future Work
This project provides a solid foundation for many advanced features:

Advanced Error Reporting: Use the line and column numbers from the lexer to provide precise error messages.

A Real Symbol Table: Implement a scope-aware symbol table to manage variable declarations and lookups.

Control Flow: Add if/else, while, and for statements.

More Types: Introduce support for strings, booleans, and arrays.
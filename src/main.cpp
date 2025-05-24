#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "IRGenerator.h" // <-- INCLUDE
#include "IR.h"          // <-- INCLUDE for the printer
#include "CodeGenerator.h"
// ... includes ...

int main() {
    std::string source = "let result = my_func(10.5, 20.5);";
    std::cout << "--- Source Code ---\n" << source << "\n\n";

    try {
        // 1. Lexing
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parsing
        Parser parser(tokens);
        std::vector<std::unique_ptr<StatementNode>> ast = parser.parse();

        // 3. Semantic Analysis
        TypeChecker typeChecker;
        typeChecker.analyze(ast);
        std::cout << "--- Semantic Analysis Complete ---\n\n";

        // 4. NEW: Intermediate Representation Generation
        IRGenerator irGenerator;
        IRProgram ir_program = irGenerator.generate(ast);
        print_ir(ir_program); // Print the generated IR

        CodeGenerator codeGenerator("output.s"); // Create the assembly file
        codeGenerator.generate(ir_program);

    std::cout << "Assembly code generated in output.s\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#pragma once

#include "AST.h" // For TokenType and DataType
#include <string>
#include <variant>
#include <vector>
#include <iostream>
// An Operand can be a temporary variable (like "t1"), a user-defined variable, or a literal constant.
using IROperand = std::variant<std::string, int, double>;

// A single Three-Address Code instruction
struct IRInstruction {
    TokenType op; // The operator (e.g., TOKEN_PLUS, TOKEN_STAR, TOKEN_EQUALS for assignment)
    
    IROperand arg1;
    IROperand arg2;
    IROperand result; // Where the result is stored (usually a temporary like "t1" or a variable name)
};

// A simple container for our entire IR program
struct IRProgram {
    std::vector<IRInstruction> instructions;
};
// In src/IR.h, after the struct definitions



// Helper to print a single operand
inline void print_operand(const IROperand& operand) {
    std::visit([](auto&& arg){ std::cout << arg; }, operand);
}

// Helper to print the entire IR program
inline void print_ir(const IRProgram& program) {
    std::cout << "--- Intermediate Representation (IR) ---\n";
    for (const auto& instr : program.instructions) {
        switch (instr.op) {
            case TokenType::CAST:
                print_operand(instr.result);
                std::cout << " = (cast) ";
                print_operand(instr.arg1);
                // We could store the target type in arg2 if needed
                break;
            case TokenType::CALL:
                print_operand(instr.result);
                std::cout << " = CALL ";
                print_operand(instr.arg1); // Function name
                // We can check the variant index for the number of args
                std::cout << ", " << std::get<int>(instr.arg2) << "_params"; 
                break;
            case TokenType::PARAM:
                std::cout << "PARAM ";
                print_operand(instr.arg1);
                break;
            case TokenType::EQUALS:
                print_operand(instr.result);
                std::cout << " = ";
                print_operand(instr.arg1);
                break;
            default: // For binary ops like +, -, *
                print_operand(instr.result);
                std::cout << " = ";
                print_operand(instr.arg1);
                std::cout << " " << (char)instr.op << " ";
                print_operand(instr.arg2);
                break;
        }
        std::cout << "\n";
    }
    std::cout << "-------------------------------------\n";
}
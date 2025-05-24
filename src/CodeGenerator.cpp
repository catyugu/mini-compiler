#include "CodeGenerator.h"
#include <vector>
#include <iostream>

// Helper function to get the correct assembly operand string.
// This is a robust version that handles all cases correctly.
std::string get_operand_asm(const IROperand& operand, const std::map<std::string, int>& stack_offsets, const std::map<std::string, std::string>& temp_registers) {
    // Case 1: The operand is a literal integer.
    if (auto val = std::get_if<int>(&operand)) {
        return std::to_string(*val);
    }
    // Case 2: The operand is a literal double (we truncate to int for now).
    if (auto val = std::get_if<double>(&operand)) {
        return std::to_string(static_cast<int>(*val));
    }
    // Case 3: The operand is a string (variable name or temporary).
    if (auto var_name = std::get_if<std::string>(&operand)) {
        // Check if it's a temporary stored in a register.
        if (temp_registers.count(*var_name)) {
            return temp_registers.at(*var_name);
        } else {
            // Otherwise, it must be a variable on the stack.
            int offset = stack_offsets.at(*var_name);
            
            // Correctly format the stack address.
            if (offset < 0) {
                return "[rbp" + std::to_string(offset) + "]";
            } else if (offset > 0) {
                return "[rbp+" + std::to_string(offset) + "]";
            } else {
                return "[rbp]";
            }
        }
    }
    return "UNKNOWN_OPERAND";
}


CodeGenerator::CodeGenerator(const std::string& output_filename) {
    m_output_file.open(output_filename);
    if (!m_output_file.is_open()) {
        throw std::runtime_error("Could not open output file for code generation.");
    }
}

void CodeGenerator::generate(const IRProgram& program) {
    // --- Boilerplate Assembly Header ---
    m_output_file << "section .text\n";
    m_output_file << "extern my_func\n\n";
    m_output_file << "global _start\n\n";
    m_output_file << "_start:\n";
    m_output_file << "    push rbp\n";
    m_output_file << "    mov rbp, rsp\n\n";

    // --- First Pass: Find all variables and allocate stack space ---
    for (const auto& instr : program.instructions) {
        if (instr.op == TokenType::EQUALS) {
            if (auto var_name = std::get_if<std::string>(&instr.result)) {
                if (m_stack_offsets.find(*var_name) == m_stack_offsets.end()) {
                    allocate_variable(*var_name);
                }
            }
        }
    }
    if (m_current_stack_offset != 0) {
        m_output_file << "    sub rsp, " << -m_current_stack_offset << "\n\n";
    }

    // --- Second Pass: Translate IR instructions to Assembly ---
    std::map<std::string, std::string> temp_registers;

    for (const auto& instr : program.instructions) {
        switch (instr.op) {
            case TokenType::PLUS:
            case TokenType::MINUS:
            case TokenType::STAR:
            case TokenType::SLASH: {
                std::string left_asm = get_operand_asm(instr.arg1, m_stack_offsets, temp_registers);
                std::string right_asm = get_operand_asm(instr.arg2, m_stack_offsets, temp_registers);
                
                m_output_file << "    mov rax, " << left_asm << "\n";
                m_output_file << "    mov rbx, " << right_asm << "\n";

                if (instr.op == TokenType::PLUS)  m_output_file << "    add rax, rbx\n";
                if (instr.op == TokenType::MINUS) m_output_file << "    sub rax, rbx\n";
                if (instr.op == TokenType::STAR)  m_output_file << "    imul rax, rbx\n";
                if (instr.op == TokenType::SLASH) {
                    m_output_file << "    xor rdx, rdx\n";
                    m_output_file << "    idiv rbx\n";
                }
                
                temp_registers[std::get<std::string>(instr.result)] = "rax";
                break;
            }
            case TokenType::EQUALS: {
                std::string dest_asm = get_operand_asm(instr.result, m_stack_offsets, temp_registers);
                std::string source_asm = get_operand_asm(instr.arg1, m_stack_offsets, temp_registers);

                m_output_file << "    mov rax, " << source_asm << "\n";
                m_output_file << "    mov " << dest_asm << ", rax\n";
                break;
            }
            case TokenType::PARAM: {
                std::string param_asm = get_operand_asm(instr.arg1, m_stack_offsets, temp_registers);
                m_output_file << "    mov rax, " << param_asm << "\n";
                m_output_file << "    push rax\n";
                break;
            }
            case TokenType::CALL: {
                std::string callee_name = std::get<std::string>(instr.arg1);
                int num_args = std::get<int>(instr.arg2);

                // NEW: Pop arguments from the stack into the correct registers
                // according to the x86-64 System V ABI.
                // Note: We pushed them in reverse, so we pop them in order.
                if (num_args > 0) {
                    m_output_file << "    pop rdi\n"; // First argument goes into RDI
                }
                if (num_args > 1) {
                    m_output_file << "    pop rsi\n"; // Second argument goes into RSI
                }
                // (A more complete implementation would handle rdx, rcx, r8, r9 here)

                m_output_file << "    call " << callee_name << "\n";
                
                // The stack cleanup `add rsp, ...` is NO LONGER NEEDED,
                // because we already cleaned it up with the pop instructions.
                
                // The return value is in rax. Map the result temporary to "rax".
                temp_registers[std::get<std::string>(instr.result)] = "rax";
                break;
            }
            case TokenType::CAST: {
                std::string source_asm = get_operand_asm(instr.arg1, m_stack_offsets, temp_registers);
                m_output_file << "    mov rax, " << source_asm << "\n";
                temp_registers[std::get<std::string>(instr.result)] = "rax";
                break;
            }
            default:
                break;
        }
        m_output_file << "\n";
    }

    // --- Boilerplate Assembly Footer ---
    m_output_file << "    ; Exit program with the value of the last variable as exit code\n";
    if (!m_stack_offsets.empty()) {
         m_output_file << "    mov rdi, [rbp" << m_current_stack_offset << "]\n";
    } else {
         m_output_file << "    xor rdi, rdi\n"; // No variables, exit with 0
    }
    m_output_file << "    mov rsp, rbp\n";
    m_output_file << "    pop rbp\n";
    m_output_file << "    mov rax, 60\n";
    m_output_file << "    syscall\n";
}

void CodeGenerator::allocate_variable(const std::string& var_name) {
    // Correct logic: decrement first, then assign.
    m_current_stack_offset -= 8;
    m_stack_offsets[var_name] = m_current_stack_offset;
    m_output_file << "    ; Allocating " << var_name << " at [rbp" << m_current_stack_offset << "]\n";
}
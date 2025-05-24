#pragma once

#include "IR.h"
#include <string>
#include <fstream>
#include <map>

class CodeGenerator {
public:
    // The constructor will open the output file.
    CodeGenerator(const std::string& output_filename);

    // The main method to generate the assembly code from the IR.
    void generate(const IRProgram& program);

private:
    std::ofstream m_output_file;
    std::map<std::string, int> m_stack_offsets; // Maps variable names to stack offsets
    int m_current_stack_offset = 0;

    // Helper to allocate space for a variable on the stack.
    void allocate_variable(const std::string& var_name);
};
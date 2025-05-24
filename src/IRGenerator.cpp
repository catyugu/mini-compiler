#include "IRGenerator.h"
#include <string>

// The main entry point. It runs the generator and returns the completed program.
IRProgram IRGenerator::generate(const std::vector<std::unique_ptr<StatementNode>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return m_program;
}

// Helper to create new, unique temporary variable names like "t0", "t1", etc.
std::string IRGenerator::new_temporary() {
    return "t" + std::to_string(m_temp_counter++);
}

// --- Visitor Implementations ---

// When we visit a statement, we just need to process the expressions inside it.
void IRGenerator::visit(const ExpressionStatementNode& node) {
    node.expression->accept(*this); // This will generate code for the expression.
}

// Literals and identifiers are the "leaves" of our expressions.
// They don't generate instructions themselves. They just provide their value or name
// to be used by their parent node. We store this in `m_last_operand`.

void IRGenerator::visit(const IntegerLiteralNode& node) {
    m_last_operand = (int)node.value;
}

void IRGenerator::visit(const FloatLiteralNode& node) {
    m_last_operand = node.value;
}

void IRGenerator::visit(const IdentifierNode& node) {
    m_last_operand = node.name;
}

// This is the core of expression code generation.
void IRGenerator::visit(const BinaryOpNode& node) {
    // 1. Recursively generate code for the left-hand side.
    // The result (e.g., a constant, or the name of a temporary "t0") will be in m_last_operand.
    node.left->accept(*this);
    IROperand left_operand = m_last_operand;

    // 2. Do the same for the right-hand side.
    node.right->accept(*this);
    IROperand right_operand = m_last_operand;

    // 3. Create a new temporary variable to store the result of this operation.
    std::string result_temp = new_temporary();

    // 4. Emit the instruction.
    m_program.instructions.push_back({
        node.op,          // The operator, e.g., TOKEN_PLUS
        left_operand,     // The first argument
        right_operand,    // The second argument
        result_temp       // The destination
    });

    // 5. Finally, update m_last_operand so that this node's parent
    // knows where to find the result of this sub-expression.
    m_last_operand = result_temp;
}

// 'let' statements use the result of an expression.
void IRGenerator::visit(const LetStatementNode& node) {
    // 1. Generate all the code for the initializer expression.
    // After this call, m_last_operand will hold the final result of the expression
    // (e.g., a constant like 5, or a temporary like "t2").
    node.initializer->accept(*this);

    // 2. Emit one final assignment instruction to move the result into the variable.
    m_program.instructions.push_back({
        TokenType::EQUALS,  // Our "assignment" operator
        m_last_operand,     // The source value
        {},                 // Assignment only has one argument, so arg2 is empty.
        node.name->name     // The destination variable
    });
}
// In src/IRGenerator.cpp

void IRGenerator::visit(const CastNode& node) {
    // 1. First, generate the code for the expression being cast.
    // The result will be in m_last_operand.
    node.expression->accept(*this);
    IROperand source_operand = m_last_operand;

    // 2. Create a new temporary to hold the result of the cast.
    std::string result_temp = new_temporary();

    // 3. Emit the CAST instruction.
    // We can store the target type in arg2 for the final code generator,
    // but for now, we'll just emit the cast operation itself.
    m_program.instructions.push_back({
        TokenType::CAST,
        source_operand,
        {}, // Not a binary operation
        result_temp
    });

    // 4. Update m_last_operand with the result of this cast.
    m_last_operand = result_temp;
}
// In src/IRGenerator.cpp

void IRGenerator::visit(const FunctionCallNode& node) {
    // 1. Generate code for each argument and emit a PARAM instruction for it.
    // We process arguments in reverse for some common calling conventions (like cdecl).
    for (int i = node.arguments.size() - 1; i >= 0; --i) {
        node.arguments[i]->accept(*this);
        m_program.instructions.push_back({
            TokenType::PARAM,
            m_last_operand, // The result of the argument expression
            {},
            {}
        });
    }

    // 2. Generate code for the callee itself (in case it's a complex expression).
    node.callee->accept(*this);
    IROperand callee_operand = m_last_operand;

    // 3. Create a new temporary to hold the return value of the function.
    std::string result_temp = new_temporary();
    
    // 4. Emit the CALL instruction. We'll store the number of arguments in arg2.
    m_program.instructions.push_back({
        TokenType::CALL,
        callee_operand,                             // The function to call
        (int)node.arguments.size(),                 // The number of params
        result_temp                                 // Where the return value goes
    });

    // 5. The result of this entire expression is the return value.
    m_last_operand = result_temp;
}
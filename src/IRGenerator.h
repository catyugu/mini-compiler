#pragma once

#include "AST.h"
#include "IR.h"

// This visitor walks the AST and generates a linear sequence of Three-Address Code.
class IRGenerator : public ASTVisitor {
public:
    IRProgram generate(const std::vector<std::unique_ptr<StatementNode>>& statements);

    // We only need to visit nodes that generate code.
    void visit(const LetStatementNode& node) override;
    void visit(const ExpressionStatementNode& node) override;
    void visit(const BinaryOpNode& node) override;
    void visit(const IntegerLiteralNode& node) override;
    void visit(const FloatLiteralNode& node) override;
    void visit(const IdentifierNode& node) override;
    void visit(const CastNode& node) override;
    void visit(const FunctionCallNode& node) override;

    // We don't have these yet, but for completeness
    // void visit(const FunctionCallNode& node) override {}
    // void visit(const CastNode& node) override {}
    // void visit(const BlockStatementNode& node) override {}
    // void visit(const IfStatementNode& node) override {}
    // void visit(const WhileStatementNode& node) override {}

private:
    IRProgram m_program;
    int m_temp_counter = 0;

    // Helper to create new temporary variable names like "t0", "t1", etc.
    std::string new_temporary();

    // When visiting an expression, the result of that expression will be stored here.
    IROperand m_last_operand;
};
#pragma once

#include "AST.h"
#include <iostream>

// The TypeChecker class will walk the AST and determine the type of each expression.
class TypeChecker : public ASTVisitor {
public:
    // Run the analysis on a complete program (a list of statements).
    ~TypeChecker() = default;
    void analyze(const std::vector<std::unique_ptr<StatementNode>>& statements);

    // Override the visit method for each node type we have implemented.
    void visit(const LetStatementNode& node) override;
    void visit(const ExpressionStatementNode& node) override;
    void visit(const FunctionCallNode& node) override;
    void visit(const BinaryOpNode& node) override;
    void visit(const IntegerLiteralNode& node) override;
    void visit(const FloatLiteralNode& node) override;
    void visit(const IdentifierNode& node) override;
    void visit(const CastNode& node) override;
};
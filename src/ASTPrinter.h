#pragma once
#include "AST.h"
#include <iostream>
inline std::ostream& operator<<(std::ostream& os, const DataType& type) {
    switch (type) {
        case DataType::INT:   os << "INT";   break;
        case DataType::FLOAT: os << "FLOAT"; break;
        case DataType::VOID:  os << "VOID";  break;
        default:              os << "UNKNOWN"; break;
    }
    return os;
}

class ASTPrinter : public ASTVisitor {
private:
    int indent_level = 0;
    void indent() { for (int i = 0; i < indent_level; ++i) std::cout << "  "; }

public:
    void print(const std::vector<std::unique_ptr<StatementNode>>& statements) {
        std::cout << "--- Abstract Syntax Tree ---\n";
        for (const auto& stmt : statements) {
            stmt->accept(*this);
        }
        std::cout << "--------------------------\n";
    }

    void visit(const LetStatementNode& node) override {
        indent();
        std::cout << "LetStatement:\n";
        indent_level++;
        indent();
        std::cout << "Name: " << node.name->name << "\n";
        indent();
        std::cout << "Initializer:\n";
        indent_level++;
        node.initializer->accept(*this);
        indent_level -= 2;
    }

    void visit(const ExpressionStatementNode& node) override {
        indent();
        std::cout << "ExpressionStatement:\n";
        indent_level++;
        node.expression->accept(*this);
        indent_level--;
    }

    // In the ASTPrinter class, modify the visit methods for expressions
    // to print the type stored in the node.
    void visit(const BinaryOpNode& node) override {
        indent();
        // NEW: Print the computed type of the expression!
        std::cout << "BinaryOp(" << node.op << ") [type: " << node.type << "]\n"; 
        // ...
    }
// (Do the same for Integer, Float, and Identifier nodes)
    void visit(const IntegerLiteralNode& node) override {
        indent();
        std::cout << "IntegerLiteral(" << node.value << ") [type: " << node.type << "]\n";
    }
    void visit(const FloatLiteralNode& node) override {
        indent();
        std::cout << "FloatLiteral(" << node.value << ") [type: " << node.type << "]\n";
    }
    void visit(const IdentifierNode& node) override {
        indent();
        std::cout << "Identifier(" << node.name << ") [type: " << node.type << "]\n";
    }
    void visit(const FunctionCallNode& node) override {}
    void visit(const CastNode& node) override {}
};


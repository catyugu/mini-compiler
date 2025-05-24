// In src/AST.h

#pragma once

#include "Token.h"
#include <memory>
#include <vector>
#include <string>

enum class DataType {
    UNKNOWN, // Type hasn't been determined yet
    VOID,    // Represents the absence of a type, e.g., for a statement
    INT,
    FLOAT
};

// Forward declare all node types and the visitor
struct BinaryOpNode;
struct IntegerLiteralNode; 
struct FloatLiteralNode; 
struct IdentifierNode;
struct FunctionCallNode;
struct LetStatementNode;
struct ExpressionStatementNode;
struct CastNode;
// The Visitor interface, updated for our new literal types.
class ASTVisitor {
public:
    virtual ~ASTVisitor();
    virtual void visit(const BinaryOpNode& node) = 0;
    virtual void visit(const IntegerLiteralNode& node) = 0; 
    virtual void visit(const FloatLiteralNode& node) = 0;  
    virtual void visit(const IdentifierNode& node) = 0;
    virtual void visit(const FunctionCallNode& node) = 0;
    virtual void visit(const LetStatementNode& node) = 0;
    virtual void visit(const ExpressionStatementNode& node) = 0;
    virtual void visit(const CastNode& node) = 0;
};


// --- Base Node Definitions ---

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

class ExpressionNode : public ASTNode {
public:
    // CRUCIAL: Every expression node now has a field to store its type.
    // This will be filled in during a later semantic analysis phase.
    DataType type = DataType::UNKNOWN;
};

class StatementNode : public ASTNode {};


// --- Concrete Expression Nodes ---

// REPLACES NumberLiteralNode
class IntegerLiteralNode : public ExpressionNode {
public:
    long long value;
    explicit IntegerLiteralNode(long long value) : value(value) {
        // For literals, we know the type at parse time.
        type = DataType::INT;
    }

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

class FloatLiteralNode : public ExpressionNode {
public:
    double value;
    explicit FloatLiteralNode(double value) : value(value) {
        type = DataType::FLOAT;
    }

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    explicit IdentifierNode(const std::string& name) : name(name) {}
    // Note: The type of an identifier is unknown until semantic analysis.
    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

class BinaryOpNode : public ExpressionNode {
public:
    TokenType op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;

    BinaryOpNode(TokenType op, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};

class LetStatementNode : public StatementNode {
public:
    std::unique_ptr<IdentifierNode> name;
    std::unique_ptr<ExpressionNode> initializer;

    LetStatementNode(std::unique_ptr<IdentifierNode> name, std::unique_ptr<ExpressionNode> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

class ExpressionStatementNode : public StatementNode {
public:
    std::unique_ptr<ExpressionNode> expression;

    explicit ExpressionStatementNode(std::unique_ptr<ExpressionNode> expression)
        : expression(std::move(expression)) {}

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

class CastNode : public ExpressionNode {
public:
    DataType targetType; // The type we are casting to
    std::unique_ptr<ExpressionNode> expression; // The expression being cast

    CastNode(DataType targetType, std::unique_ptr<ExpressionNode> expression)
        : targetType(targetType), expression(std::move(expression)) {}

    void accept(ASTVisitor& visitor) const override { visitor.visit(*this); }
};
// In src/AST.h

class FunctionCallNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> callee;
    std::vector<std::unique_ptr<ExpressionNode>> arguments;
    FunctionCallNode(std::unique_ptr<ExpressionNode> callee, std::vector<std::unique_ptr<ExpressionNode>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {}

    void accept(ASTVisitor& visitor) const override {
        visitor.visit(*this);
    }
};
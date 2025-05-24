#include "SemanticAnalyzer.h"

void TypeChecker::analyze(const std::vector<std::unique_ptr<StatementNode>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
}

// For a statement, we just need to analyze the expressions within it.
void TypeChecker::visit(const LetStatementNode& node) {
    node.initializer->accept(*this);
}

void TypeChecker::visit(const ExpressionStatementNode& node) {
    node.expression->accept(*this);
}

// Literals are the base cases for our recursion. Their type is already known.
void TypeChecker::visit(const IntegerLiteralNode& node) {
    // The type is set in the constructor, so there's nothing to do here.
}

void TypeChecker::visit(const FloatLiteralNode& node) {
    // The type is set in the constructor, so there's nothing to do here.
}

// For now, we need to make an assumption about an identifier's type.
// A real compiler would use a "symbol table" to look this up.
void TypeChecker::visit(const IdentifierNode& node) {
    // For testing purposes, let's assume any variable found is an INT.
    const_cast<IdentifierNode&>(node).type = DataType::INT;
}

// This is the core logic for type checking expressions.
void TypeChecker::visit(const BinaryOpNode& node) {
    // 1. Recursively visit the left and right children to compute their types first.
    node.left->accept(*this);
    node.right->accept(*this);

    DataType leftType = node.left->type;
    DataType rightType = node.right->type;

    // 2. Apply our language's type rules.
    if (leftType == DataType::FLOAT || rightType == DataType::FLOAT) {
        // Type Promotion Rule: If either operand is a float, the result is a float.
        const_cast<BinaryOpNode&>(node).type = DataType::FLOAT;
    }
    else if (leftType == DataType::INT && rightType == DataType::INT) {
        // If both are ints, the result is an int.
        const_cast<BinaryOpNode&>(node).type = DataType::INT;
    }
    else {
        // The types are incompatible (e.g., UNKNOWN or some future type like STRING).
        throw std::runtime_error("Semantic Error: Incompatible types for binary operator.");
    }
}
void TypeChecker::visit(const CastNode& node) {
    // 1. First, recursively visit the inner expression to determine its original type.
    // This will fill in the `.type` field of `node.expression`.
    node.expression->accept(*this);

    // 2. Get the original (source) type and the requested target type.
    DataType sourceType = node.expression->type;
    DataType targetType = node.targetType;

    // 3. (Optional but good practice) Validate the cast.
    // In our simple language with only INT and FLOAT, most casts are valid.
    // A more complex language would have much stricter rules here.
    if (sourceType == DataType::UNKNOWN || sourceType == DataType::VOID) {
        throw std::runtime_error("Semantic Error: Cannot perform a cast from an unknown or void type.");
    }

    // You could also add warnings for potentially "lossy" conversions.
    if (sourceType == DataType::FLOAT && targetType == DataType::INT) {
        // This is a valid conversion, but may result in loss of precision.
        // A real-world compiler would typically emit a warning for the user.
        std::cout << "Warning: Potential data loss on conversion from FLOAT to INT.\n";
    }

    // 4. The type of the entire cast expression IS the target type.
    // This is the main purpose of this function.
    // We use const_cast because the visitor takes a const reference by default,
    // but our specific job here is to intentionally modify the node's type field.
    const_cast<CastNode&>(node).type = targetType;
}
// In src/SemanticAnalyzer.cpp, inside the TypeChecker class

void TypeChecker::visit(const FunctionCallNode& node) {
    // 1. Recursively visit the callee and all arguments to ensure their types are computed first.
    node.callee->accept(*this);
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
    }

    // 2. Determine the function's return type.
    // In a real compiler, you would look up node.callee in a symbol table
    // to find its signature and return type.
    // For now, as a placeholder, we'll just assume any function call returns an INT.
    const_cast<FunctionCallNode&>(node).type = DataType::INT;
}

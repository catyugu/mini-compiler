#include "Parser.h"
// In src/Parser.cpp

// In src/Parser.cpp

std::unique_ptr<ExpressionNode> Parser::parsePrimary() {
    if (match({TokenType::FLOAT_LITERAL})) {
        double value = std::stod(previous().lexeme);
        return std::make_unique<FloatLiteralNode>(value);
    }
    if (match({TokenType::INTEGER_LITERAL})) {
        long long value = std::stoll(previous().lexeme);
        return std::make_unique<IntegerLiteralNode>(value);
    }
    if (match({TokenType::IDENTIFIER})) {
        // This is just a plain identifier, not a call or cast
        return std::make_unique<IdentifierNode>(previous().lexeme);
    }

    if (match({TokenType::LEFT_PAREN})) {
        // --- THIS IS THE NEW, SMARTER LOGIC ---
        // Check if this is a cast: (IDENTIFIER) ...
        if (peek().type == TokenType::IDENTIFIER && m_tokens[m_current + 1].type == TokenType::RIGHT_PAREN) {
            // Check if the identifier is a known type name
            if (known_type_names.count(peek().lexeme) > 0) {
                advance(); // Consume the identifier (the type name)
                const Token type_token = previous();
                DataType targetType = (type_token.lexeme == "int") ? DataType::INT : DataType::FLOAT;

                consume(TokenType::RIGHT_PAREN, "Expected ')' after type name in cast.");
                
                // Parse the expression to be casted. IMPORTANT: Use parseCall() to allow (int)my_func()
                std::unique_ptr<ExpressionNode> exprToCast = parseCall();
                return std::make_unique<CastNode>(targetType, std::move(exprToCast));
            }
        }
        
        // If it's not a cast, it's a normal grouped expression.
        std::unique_ptr<ExpressionNode> expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return expr;
    }

    throw std::runtime_error("Parser Error: Unexpected token '" + peek().lexeme + "' when expecting an expression.");
}
// In src/Parser.cpp

std::unique_ptr<ExpressionNode> Parser::parseMultiplication() {
    // MODIFIED: 调用 parseCall() 而不是 parsePrimary()
    std::unique_ptr<ExpressionNode> expr = parseCall();

    while (match({TokenType::STAR, TokenType::SLASH})) {
        const Token op = previous();
        // MODIFIED: 调用 parseCall() 而不是 parsePrimary()
        std::unique_ptr<ExpressionNode> right = parseCall();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}
// In src/Parser.cpp

std::unique_ptr<ExpressionNode> Parser::parseAddition() {
    // The left-hand side can be a full multiplication/division expression.
    std::unique_ptr<ExpressionNode> expr = parseMultiplication();

    while (match({TokenType::PLUS, TokenType::MINUS})) {
        const Token op = previous();
        std::unique_ptr<ExpressionNode> right = parseMultiplication();
        expr = std::make_unique<BinaryOpNode>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

// And define the top-level expression parser to start the chain.
std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    // For now, the lowest precedence level is addition.
    // In the future, this could be assignment, e.g., `return parseAssignment();`
    return parseAddition();
}
// In src/Parser.cpp

// An ExpressionStatement is just an expression followed by a semicolon.
std::unique_ptr<StatementNode> Parser::parseExpressionStatement() {
    std::unique_ptr<ExpressionNode> expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return std::make_unique<ExpressionStatementNode>(std::move(expr));
}

std::unique_ptr<StatementNode> Parser::parseLetStatement() {
    // The 'let' keyword has already been consumed by parseStatement().
    const Token nameToken = consume(TokenType::IDENTIFIER, "Expected variable name after 'let'.");
    auto name = std::make_unique<IdentifierNode>(nameToken.lexeme);

    consume(TokenType::EQUALS, "Expected '=' after variable name.");

    std::unique_ptr<ExpressionNode> initializer = parseExpression();
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    
    return std::make_unique<LetStatementNode>(std::move(name), std::move(initializer));
}

// This is the dispatcher that chooses the correct statement parser.
std::unique_ptr<StatementNode> Parser::parseStatement() {
    if (match({TokenType::LET})) {
        return parseLetStatement();
    }

    // If no other statement type matches, assume it's an expression statement.
    return parseExpressionStatement();
}

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::vector<std::unique_ptr<StatementNode>> Parser::parse() {
    std::vector<std::unique_ptr<StatementNode>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(parseStatement());
        } catch (const std::runtime_error& e) {
            // 这里可以添加错误恢复逻辑，但现在我们先简单打印并退出
            std::cerr << e.what() << std::endl;
            return {}; // 或者根据需要处理
        }
    }
    return statements;
}


// --- Helper/Utility Methods (您缺失的部分) ---

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
    return m_tokens[m_current];
}

const Token& Parser::previous() const {
    // m_current 总是在下一个token，所以前一个在 m_current - 1
    return m_tokens[m_current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        m_current++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error("Parser Error: " + message + " (at token '" + peek().lexeme + "')");
}

// 在 src/Parser.cpp 中

// NEW: 实现 parseCall 函数
std::unique_ptr<ExpressionNode> Parser::parseCall() {
    // 先解析一个 primary 表达式，这可能是个函数名
    std::unique_ptr<ExpressionNode> expr = parsePrimary();

    // 循环检查后面是否跟随着'(', 以支持 f(x)() 这种调用
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            // 如果是'(', 说明这是一个函数调用
            std::vector<std::unique_ptr<ExpressionNode>> args = parseArguments();
            expr = std::make_unique<FunctionCallNode>(std::move(expr), std::move(args));
        } else {
            break; // 不是函数调用，退出循环
        }
    }
    return expr;
}

// NEW: 实现参数列表的辅助解析函数
std::vector<std::unique_ptr<ExpressionNode>> Parser::parseArguments() {
    std::vector<std::unique_ptr<ExpressionNode>> args;
    
    // 如果括号内不是空的
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match({TokenType::COMMA})); // 循环解析用逗号分隔的参数
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments.");
    return args;
}

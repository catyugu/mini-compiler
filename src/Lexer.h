#pragma once

#include <string>
#include <vector>
#include "Token.h"

class Lexer {
public:
    // Constructor: takes the source code to be tokenized
    Lexer(const std::string& source);

    // The main function that generates all tokens from the source
    std::vector<Token> tokenize();

private:
    std::string m_source;
    size_t m_start = 0;
    size_t m_current = 0;
    int m_line = 1;

    // Helper methods
    bool isAtEnd() const;
    char advance();
    Token makeToken(TokenType type);
    Token makeErrorToken(const std::string& message);
    void skipWhitespace();
    Token scanToken();
    Token identifier();
    Token scanNumber();
    char peek() const;                      // Safely look at the current character
    char peekNext() const;                  // Safely look at the next character
    Token number();
};
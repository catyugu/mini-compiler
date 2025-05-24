# pragma once
#include <string>
#include <ostream>

using namespace std;

// Represents the type of a token
enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, COLON,
    SEMICOLON, EQUALS,
    PLUS, MINUS, STAR, SLASH,
    CAST,
    PARAM, // Represents passing a parameter to a function
    CALL,

    // Keywords
    LET,

    // Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,


    // Special
    UNKNOWN, // For unrecognized characters
    END_OF_FILE
};

// A structure to hold information about a single token
struct Token {
    TokenType type;
    std::string lexeme; // The actual text of the token (e.g., "let", "myVar", "42")
    int line;           // The line number where the token appears
    int column;         // The column number where the token starts
};

// A helper function to easily print a token's type (useful for debugging)
// This lets us do `std::cout << token.type;`
std::ostream& operator<<(std::ostream& os, const TokenType& type);
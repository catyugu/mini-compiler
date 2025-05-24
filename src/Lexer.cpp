#include "Lexer.h"
#include <map>

// Keywords mapping
static const std::map<std::string, TokenType> keywords = {
    {"let", TokenType::LET}
};

Lexer::Lexer(const std::string& source) : m_source(source) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        m_start = m_current;
        Token token = scanToken();
        tokens.push_back(token);
    }
    // Add one final EOF token
    tokens.push_back({TokenType::END_OF_FILE, "", m_line, (int)m_source.length()});
    return tokens;
}

bool Lexer::isAtEnd() const{
    return m_current >= m_source.length();
}

// Consumes the current character and returns it
char Lexer::advance() {
    m_current++;
    return m_source[m_current - 1];
}

Token Lexer::makeToken(TokenType type) {
    std::string lexeme = m_source.substr(m_start, m_current - m_start);
    return {type, lexeme, m_line, (int)m_start};
}

Token Lexer::makeErrorToken(const std::string& message) {
    return {TokenType::UNKNOWN, message, m_line, (int)m_start};
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = m_source[m_current];
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                m_line++;
                advance();
                break;
            default:
                return;
        }
    }
}
char Lexer::peek() const {
    if (m_current >= m_source.length()) return '\0'; // Return null terminator if at the end
    return m_source[m_current];
}

char Lexer::peekNext() const {
    // Check if the *next* character is out of bounds
    if (m_current + 1 >= m_source.length()) return '\0';
    return m_source[m_current + 1];
}
Token Lexer::scanNumber() {
    while (isdigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isdigit(peekNext())) {
        // Consume the "."
        advance();

        while (isdigit(peek())) advance();
        return makeToken(TokenType::FLOAT_LITERAL);
    }

    return makeToken(TokenType::INTEGER_LITERAL);
}

Token Lexer::identifier() {
    while (isalnum(m_source[m_current]) || m_source[m_current] == '_') advance();

    std::string text = m_source.substr(m_start, m_current - m_start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return makeToken(it->second); // It's a keyword
    }

    return makeToken(TokenType::IDENTIFIER);
}


Token Lexer::scanToken() {
    skipWhitespace();
    m_start = m_current;

    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE);

    char c = advance();

    if (isalpha(c) || c == '_') return identifier();
    if (isdigit(c)) return scanNumber(); // Make sure this calls scanNumber()

    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '[': return makeToken(TokenType::LEFT_BRACKET);
        case ']': return makeToken(TokenType::RIGHT_BRACKET);
        case ',': return makeToken(TokenType::COMMA);
        case '.': return makeToken(TokenType::DOT);
        case ':': return makeToken(TokenType::COLON);
        case ';': return makeToken(TokenType::SEMICOLON);
        case '=': return makeToken(TokenType::EQUALS);
        case '+': return makeToken(TokenType::PLUS);
        case '-': return makeToken(TokenType::MINUS);
        case '*': return makeToken(TokenType::STAR);
        case '/': return makeToken(TokenType::SLASH);
    }

    return makeErrorToken("Unexpected character.");
}
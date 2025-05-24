#include "Token.h"

std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    switch (type) {
        case TokenType::LEFT_PAREN:   os << "LEFT_PAREN";   break;
        case TokenType::RIGHT_PAREN:  os << "RIGHT_PAREN";  break;
        case TokenType::LEFT_BRACE:   os << "LEFT_BRACE";   break;
        case TokenType::RIGHT_BRACE:  os << "RIGHT_BRACE";  break;
        case TokenType::LEFT_BRACKET: os << "LEFT_BRACKET"; break;
        case TokenType::RIGHT_BRACKET:os << "RIGHT_BRACKET";break;
        case TokenType::COMMA:        os << "COMMA";        break;
        case TokenType::DOT:          os << "DOT";          break;
        case TokenType::COLON:        os << "COLON";        break;
        case TokenType::SEMICOLON:    os << "SEMICOLON";    break;
        case TokenType::EQUALS:       os << "EQUALS";       break;
        case TokenType::PLUS:         os << "PLUS";         break;
        case TokenType::MINUS:        os << "MINUS";        break;
        case TokenType::STAR:         os << "STAR";         break;
        case TokenType::SLASH:        os << "SLASH";        break;
        case TokenType::LET:          os << "LET";          break;
        case TokenType::IDENTIFIER:   os << "IDENTIFIER";   break;
        case TokenType::INTEGER_LITERAL: os << "INTEGER_LITERAL"; break;
        case TokenType::FLOAT_LITERAL: os << "FLOAT_LITERAL"; break;
        case TokenType::UNKNOWN:      os << "UNKNOWN";      break;
        case TokenType::END_OF_FILE:  os << "END_OF_FILE";  break;
    }
    return os;
}
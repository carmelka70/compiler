#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <string>
#include <optional>
#include <string_view>
#include <unordered_map>

#include "compiler.h"
#include "token.h"

namespace Compiler {
class Lexer
{
    public:
        Lexer(const CompileContext& context);
        ~Lexer() = default;

        std::optional<Token> getNextToken();
        std::optional<Token> peekNextToken();

        void printTokenKey(Token token);

    private:
        std::ifstream ifs {};
        std::string line {};
        std::optional<Token> nextToken;

        size_t atLine = 0;
        size_t atColumn = 0;

        const std::unordered_map<std::string_view ,TokenType> kKeywords =
        {
            {"define" ,TokenType::Define},
            {"new" ,TokenType::New},
            {"create" ,TokenType::Create},
            {"delete" ,TokenType::Delete},
            {"return" ,TokenType::Return},

            {"undefined" ,TokenType::Undefined},
            {"nan" ,TokenType::Nan},
            {"null" ,TokenType::Null},
            {"and" ,TokenType::And},
            {"or" ,TokenType::Or},
            {"not" ,TokenType::Not},
            {"xor" ,TokenType::Xor},
        };

        const std::unordered_map<std::string_view ,TokenType> kOperators =
        {
            {"=" ,TokenType::Assign},
            {":" ,TokenType::Allocate},
            {":=" ,TokenType::Reference},
            {";" ,TokenType::Semicolon},

            {"{" ,TokenType::LBrace},
            {"}" ,TokenType::RBrace},
            {"(" ,TokenType::LParen},
            {")" ,TokenType::RParen},
            {"[" ,TokenType::LBracket},
            {"]" ,TokenType::RBracket},

            {"+" ,TokenType::Plus},
            {"-" ,TokenType::Minus},
            {"*" ,TokenType::Multiplication},
            {"/" ,TokenType::Division},
            {"%" ,TokenType::Modulo},

            {"+=" ,TokenType::PlusEquals},
            {"-=" ,TokenType::MinusEquals},
            {"*=" ,TokenType::MultiplicationEquals},
            {"/=" ,TokenType::DivisionEquals},
            {"%=" ,TokenType::ModuloEquals},

            {"++" ,TokenType::DoublePlus},
            {"--" ,TokenType::DoubleMinus},

            {"==" ,TokenType::Equals},
            {"!=" ,TokenType::NotEquals},
            {">" ,TokenType::GreaterThan},
            {"<" ,TokenType::LessThan},
            {">=" ,TokenType::GreaterEquals},
            {"<=" ,TokenType::LessEquals},
        };

        Token lexIdentifierOrToken(const std::string_view view);

        Token lexNumber(const std::string_view view);

        std::string escapeString(std::string_view input);
        Token lexCharOrString(const std::string_view view);
        Token lexString(const std::string_view view);

        Token lexOperator(const std::string_view view);

        std::optional<Token> tokenizeAtPosition();
        void advancePosition(const std::optional<Token> token);
};

}; //Compiler

#endif

#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <string>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>

#include "compiler.h"

namespace Compiler {

enum class TokenType
{
    Unknown = 0,
    Identifier,

    // Data types
    Integer,
    Double,
    Char,
    String,

    // Keywords
    Define,
    New,
    Create,
    Delete,
    Return,

    Undefined, // Values
    Nan,
    Null,

    Or, // Logic
    And,
    Not,
    Xor,

    // Operators
    Assign,
    Reference,
    Declare,
    Semicolon,

    LBrace, //
    RBrace,
    LParen,
    RParen,
    LBracket,
    RBracket,

    Plus, // Arithmetic
    Minus,
    Multiplication,
    Division,
    Modulo,

    PlusEquals,
    MinusEquals,
    MultiplicationEquals,
    DivisionEquals,
    ModuloEquals,

    DoublePlus,
    DoubleMinus,

    Equals, // Logic
    NotEquals,
    GreaterThan,
    LessThan,
    GreaterEquals,
    LessEquals,

};

using Int_t = long long;
using Double_t = double;
using Char_t = char;
using String_t = std::string;
using Identifier_t = String_t;

struct Token
{
    TokenType type {TokenType::Unknown};
    std::variant<
        std::monostate,
        Int_t,
        Double_t,
        Char_t,
        String_t
            > value {};
    size_t line {};
    size_t column {};

};

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
            {":" ,TokenType::Declare},
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

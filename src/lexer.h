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
    // Operators
    Assign,
    Reference,
    Declare,
    Semicolon,
};

using Int_t = long long;
using Double_t = long double;
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
        size_t position = 0;
        std::optional<Token> nextToken;

        size_t atLine = 0;
        size_t atColumn = 0;

        const std::unordered_map<std::string_view ,TokenType> kKeywords =
        {
            {"define" ,TokenType::Define},
            {"new" ,TokenType::New},
            {"create" ,TokenType::Create},
            {"delete" ,TokenType::Delete},
        };

        const std::unordered_map<std::string_view ,TokenType> kOperators =
        {
            {"=" ,TokenType::Assign},
            {":" ,TokenType::Declare},
            {":=" ,TokenType::Reference},
            {";" ,TokenType::Semicolon},
        };

        std::optional<Token> lexIdentifierOrToken(const std::string_view view);
        std::optional<Token> lexNumber(const std::string_view view);
        std::optional<Token> lexCharOrString(const std::string_view view);
        std::optional<Token> lexString(const std::string_view view);
        std::optional<Token> lexOperator(const std::string_view view);

        std::optional<Token> tokenizeAtPosition();
        void advancePosition(const std::optional<Token> token);
};

}; //Compiler

#endif

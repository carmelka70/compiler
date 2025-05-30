#ifndef TOKEN_H
#define TOKEN_H

#include <variant>
#include <string>

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

}; // Compiler

#endif

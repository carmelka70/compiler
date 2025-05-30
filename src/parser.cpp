#include "parser.h"
#include "compiler.h"
#include <utility>

using namespace Compiler;

Parser::Parser(const CompileContext& context)
{
}

Parser::Parser(const Parser& parser)
{
    this->nestLevel = parser.nestLevel + 1;
}

void Parser::collect(Token token)
{ // TODO make a state machine
    switch (token.type)
    {
        case TokenType::Semicolon:
            isStatementReady = true;
            break;
        case TokenType::LBrace:
        case TokenType::LBracket:
        case TokenType::LParen:
        {
            Parser parser(*this); 
            break;
        }
        default:
           tokenStream.emplace_back(std::move(token));
    }
    if (token.type == TokenType::Semicolon)
        isStatementReady = true;
    else
        tokenStream.emplace_back(std::move(token));
}

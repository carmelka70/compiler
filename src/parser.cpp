#include "parser.h"
#include "astnode.h"
#include "compiler.h"
#include "token.h"

#include <iostream>
#include <memory>
#include <string>
#include <cassert>

using namespace Compiler;

Parser::Parser(const CompileContext& context)
    : kMaxNestRange_ (context.maxNestRange)
{}

Parser::Parser(const Parser& parser)
    : kMaxNestRange_ (parser.kMaxNestRange_)
    ,nestLevel_ (parser.nestLevel_)
{}




void Parser::consume(Token token)
{
    switch (token.type)
    {
        case TokenType::Semicolon:
            if (consumeNestLevel == 0) 
                isStatementReady_ = true;
            break;
        case TokenType::LBrace:
            consumeNestLevel++;
            break;
        case TokenType::RBrace:
            consumeNestLevel--;
            break;
        default:
            break;
    }
    tokenStream_.emplace_back(std::move(token));
}

bool Parser::statementReady()
{
    return isStatementReady_;
}

const Token& Parser::currentToken() const
{
    return tokenStream_[currentIndex_];
}
void Parser::advance()
{
    currentIndex_++;
}
bool Parser::match(TokenType type)
{
    return currentToken().type == type;
}

std::string Parser::currentTokenPos()
{
    return Compiler::getTokenPos(currentToken());
}

bool Parser::expect(TokenType type)
{
    bool isMatch = match(type);
    if (!isMatch)
    {
        log ("ERROR: Token missmatch at line " + currentTokenPos() + " - expected " + Compiler::getTokenKey(type) + " but got " + Compiler::getTokenKey(currentToken().type) + ".");
    }
    return isMatch;
}



std::unique_ptr<AST::Expression> Parser::parseExpression()
{return nullptr;} // TODO


std::unique_ptr<AST::ASTNode> Parser::parseVariable()
    // Specifier Identifier ; or
    // Specifier Identifier <: = :=> expression or
{
    TokenType varType = currentToken().type;
    advance();

    if (!expect(TokenType::Identifier))
        return nullptr;


    const std::string& name = std::get<std::string>(currentToken().value);
    advance();

    // TODO domain, strong typing

    bool isRuntime = (varType == TokenType::New);

    if (match(TokenType::Semicolon)) // empty decleration
        return std::make_unique<AST::VarDeclaration>(name ,isRuntime);

    TokenType valueRelation = currentToken().type;
    advance();

    auto value = parseExpression();

    /*
    if (value == nullptr) // expression is invalid
        return nullptr;
*/

    switch (valueRelation)
    {
        case TokenType::Assign: // decleration and assignment
            return std::make_unique<AST::VarDefinition>(
                    name
                    ,isRuntime
                    ,true
                    ,std::move(value));
        case TokenType::Allocate: // decleration and allocation
            return std::make_unique<AST::VarAllocation> (
                    name
                    ,isRuntime
                    ,true
                    ,std::move(value));
        case TokenType::Reference: // decleration and reference
            return std::make_unique<AST::VarReference> (
                    name
                    ,isRuntime
                    ,true
                    ,std::move(value));
        default: // invalid next token
            log ("ERROR: Invalid token \'" + Compiler::getTokenKey(valueRelation) + "\' at line " + currentTokenPos() + ": use either \'=\' \'=\' or \':=\'.");
            return nullptr;
    }
}

std::unique_ptr<AST::Block> Parser::parseBlock()
{
    if (nestLevel_ > kMaxNestRange_)
        return nullptr;


    Parser nestedParser(*this);

    auto block = std::make_unique<AST::Block>();

    advance(); // skip '{'
    
    while (currentIndex_ < tokenStream_.size())
    {
        if (tokenStream_.empty()) // missing a '}'
        {
            log ("ERROR: missing a \'}\' for brace at line " + currentTokenPos() + ".");
            return nullptr;
        }

        nestedParser.consume(currentToken());
        if (nestedParser.statementReady())
        {
            auto node = nestedParser.parse();
            if (node == nullptr)
                return nullptr;

            block->ASTList.emplace_back(std::move(node));
        }
        advance();
    }
    return block;
}

std::unique_ptr<AST::ASTNode> Parser::getAST()
{
    switch (currentToken().type)
    {
        case TokenType::Define:
        case TokenType::New:
            // Variable
            return parseVariable();
        case TokenType::LBrace:
            return parseBlock();
        default:
            return nullptr;
    }
}

std::unique_ptr<AST::ASTNode> Parser::parse()
{
    /*
    for (auto token : tokenStream_)
        log(getTokenKey(token.type));
    log("");
    */
    auto node = getAST();

    tokenStream_.clear();
    isStatementReady_ = false;
    currentIndex_ = 0;

    return node;
}


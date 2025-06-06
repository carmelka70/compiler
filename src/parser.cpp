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

void Parser::consume(Token token)
{
    switch (token.type)
    {
        case TokenType::LBrace:
            consumeNestLevel_++;
            break;
        case TokenType::RBrace:
            consumeNestLevel_--;
            break;
        case TokenType::Semicolon:
            if (consumeNestLevel_ == 0) 
                isStatementReady_ = true;
            break;
        default:
            break;
    }
    tokenStream_.emplace_back(token);
}

bool Parser::statementReady()
{
    return isStatementReady_;
}

bool Parser::statementNotEmpty()
{
    return !tokenStream_.empty();
}

const Token& Parser::currentToken() const
{
    return tokenStream_[currentIndex_];
}

TokenType Parser::currentTokenType() const
{
    return currentToken().type;
}
void Parser::advance()
{
    currentIndex_++;
}
bool Parser::match(TokenType type)
{
    return currentToken().type == type;
}

bool Parser::isTokenStreamEmpty()
{
    return currentIndex_ == tokenStream_.size() - 1;
}

std::string Parser::strCurrentTokenPos()
{
    return Compiler::getTokenPos(currentToken());
}

std::string Parser::strCurrentTokenType()
{
    return Compiler::getTokenKey(currentToken().type);
}

bool Parser::expect(TokenType type)
{
    bool isMatch = match(type);
    if (!isMatch)
    {
        log ("ERROR: Token missmatch at line " + strCurrentTokenPos() + " - expected " + Compiler::getTokenKey(type) + " but got " + strCurrentTokenType() + ".");
    }
    return isMatch;
}





std::unique_ptr<AST::ASTNode> Parser::parseEmpty()
{
    advance(); //skip ';'
    return std::make_unique<AST::Empty>();
}


std::unique_ptr<AST::Rvalue> Parser::parseRvalue()
    // { Set }
    // ( Set )
    // Expression
    // Value
{
    switch (currentTokenType())
    {
        case TokenType::LBrace: // Set
        case TokenType::LParen: // Set value OR Expression
            return parseSet();
            break;
        default:

    }
}

std::unique_ptr<AST::Rvalue> Parser::parseSet()
{
    auto setNode = std::make_unique<AST::Set>();
    setNode->isSetValue = currentTokenType() == TokenType::LParen; 

    advance(); // skip '(' or '{'

    for (auto token : tokenStream_)
        if (token.type == TokenType::Semicolon)
            return parseBlock();

}


std::unique_ptr<AST::ASTNode> Parser::parseVariable()
    // Specifier Identifier ; or
    // Specifier Identifier <: = :=> expression or
{
    TokenType varType = currentToken().type;
    advance(); // skip Specifier

    if (!expect(TokenType::Identifier))
        return nullptr;


    const std::string& name = std::get<std::string>(currentToken().value);
    advance(); // skip Identifier

    // TODO domain, strong typing

    bool isRuntime = (varType == TokenType::New);

    if (match(TokenType::Semicolon)) // empty decleration
    {
        advance(); // skip ';'
        return std::make_unique<AST::VarDeclaration>(name ,isRuntime);
    }

    TokenType valueRelation = currentToken().type;
    advance(); // skip valueRelation

    auto value = parseRvalue();

    /*
    if (value == nullptr) // expression is invalid
        return nullptr;
    */
    expect(TokenType::Semicolon);
    advance(); // skip ';'

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
            log ("ERROR: Invalid token \'" + Compiler::getTokenKey(valueRelation) + "\' at line " + strCurrentTokenPos() + ": use either \'=\' \'=\' or \':=\'.");
            return nullptr;
    }
}

std::unique_ptr<AST::Block> Parser::parseBlock()
{
    if (nestLevel_ > kMaxNestRange_)
    {
        log("ERROR: exceeded max nesting range of " + std::to_string(kMaxNestRange_) + " at line " + strCurrentTokenPos() + ".");
        return nullptr;
    }

    if (!match(TokenType::LBrace))
    {
        log("ERROR: expected '{' at line " + strCurrentTokenPos());
        return nullptr;
    }

    auto block = std::make_unique<AST::Block>();
    std::string blockStartPos = strCurrentTokenPos();

    advance(); // skip '{'
    const int kblockNestLevel = nestLevel_;
    nestLevel_++;

    while (nestLevel_ > kblockNestLevel)
    {
        if (isTokenStreamEmpty())
        {
            log("ERROR: missing closing '}' for brace at line " + blockStartPos + ".");
            return nullptr;
        }

        if (currentToken().type == TokenType::RBrace)
        {
            nestLevel_--;
            advance(); // skip '}'
            continue;
        }

        auto node = getAST();
        if (!node)
        {
            log("errors in this block"); // should err in getAST()
            return nullptr;
        }

        block->ASTList.emplace_back(std::move(node));
    }


    advance(); // consume '}'

    return block;
}

std::unique_ptr<AST::ASTNode> Parser::getAST()
{

    switch (currentToken().type)
    {
        case TokenType::Semicolon:
            return parseEmpty();
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
    auto node = getAST();

    tokenStream_.clear();
    isStatementReady_ = false;
    currentIndex_ = 0;

    return node;
}


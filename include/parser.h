#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

#include "compiler.h"
#include "token.h"
#include "astnode.h"

namespace Compiler {

class Parser
{
    public:
        Parser(const CompileContext& context);
        ~Parser() = default;

        void consume(Token token);
        bool statementReady();
        bool statementNotEmpty();
        std::unique_ptr<AST::ASTNode> parse();

    private:
        const int kMaxNestRange_;

        std::vector<Token> tokenStream_ {};
        size_t currentIndex_ = 0;
        bool isStatementReady_ = false;
        int nestLevel_ = 0;

        unsigned consumeNestLevel_ = 0;

        const Token& currentToken() const;
        TokenType currentTokenType() const;
        void advance(); // advance the vector view;
        bool match(TokenType type); // returns if token matches
        bool isTokenStreamEmpty();  // returns if at the end of tokenStream_
        std::string strCurrentTokenPos(); // returns "line:column"
        std::string strCurrentTokenType(); // returns current token type
        bool expect(TokenType type); // logs error if not matching
        
        std::unique_ptr<AST::ASTNode> getAST();

        std::unique_ptr<AST::ASTNode> parseEmpty();

        std::unique_ptr<AST::ASTNode> parseVariable();

        std::unique_ptr<AST::Rvalue> parseRvalue();
        std::unique_ptr<AST::Rvalue> parseSet();

        std::unique_ptr<AST::Block> parseBlock();
        
};

}; //Compiler

#endif

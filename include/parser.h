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
        Parser(const Parser& parser);
        ~Parser() = default;

        void consume(Token token);
        bool statementReady();
        std::unique_ptr<AST::ASTNode> parse();

    private:
        const unsigned kMaxNestRange_;

        std::vector<Token> tokenStream_ {};
        size_t currentIndex_ = 0;
        bool isStatementReady_ = false;
        unsigned nestLevel_ = 0;

        unsigned consumeNestLevel_ = 0;

        const Token& currentToken() const;
        void advance(); // advance the vector view;
        bool match(TokenType type); // returns if token matches
        std::string currentTokenPos(); // returns "line:column"
        bool expect(TokenType type); // logs error if not matching
        
        std::unique_ptr<AST::ASTNode> getAST();

        std::unique_ptr<AST::ASTNode> parseEmpty();

        std::unique_ptr<AST::ASTNode> parseVariable();
        std::unique_ptr<AST::Expression> parseExpression();

        std::unique_ptr<AST::Block> parseBlock();
        
};

}; //Compiler

#endif

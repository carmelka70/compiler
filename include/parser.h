#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
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

        void collect(Token token);
        bool statementReady();
        const AST::ASTNode& parse();

    private:
        std::vector<Token> tokenStream {};
        bool isStatementReady = false;
        unsigned nestLevel = 0;

        std::vector<std::unique_ptr<AST::ASTNode>> ASTList {};
        
};

}; //Compiler

#endif

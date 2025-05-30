#ifndef COMPILER_H
#define COMPILER_H

#include <memory>
#include <vector>
#include <string>

#include "token.h"
#include "astnode.h"

namespace Compiler {

void log(std::string_view msg);

struct CompileContext
{
    std::vector<std::string> sourceFiles {};
    std::string outputName {"out"};
    unsigned maxNestRange = 500;
    // ...
};

CompileContext generateCompilerContext(int argc ,const char *argv[]);

std::string getTokenKey(TokenType type);

std::string getTokenPos(Token token);


void printASTNode(const std::unique_ptr<AST::ASTNode>& node);

}; // Compiler

#endif

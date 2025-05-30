#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
namespace Compiler {
namespace AST {

class ASTNode {};

class DefinitionNode : ASTNode
{
    std::string name;
};

} // AST
} // Compiler


#endif

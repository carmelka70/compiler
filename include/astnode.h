#ifndef ASTNODE_H
#define ASTNODE_H

#include <memory>
#include <string>
#include <vector>


namespace Compiler {
namespace AST {

struct ASTNode {};

struct Expression : ASTNode {};

// Expression

// Variables

struct VariableBase : ASTNode
{
    const std::string name;
    bool isRuntime;
    bool isDecleration;
    std::unique_ptr<Expression> value; // nullptr for declaration

    VariableBase(const std::string& name, bool isRuntime, bool isDecleration, std::unique_ptr<Expression> value = nullptr)
        : name(std::move(name)), isRuntime(isRuntime), isDecleration(isDecleration), value(std::move(value)) {}
};

struct VarDecleration : VariableBase
{
    VarDecleration(const std::string& name, bool isRuntime)
        : VariableBase(std::move(name), isRuntime, true) {}
};

struct VarDefinition : VariableBase
{
    VarDefinition (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Expression> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}
};

struct VarAllocation : VariableBase
{
    VarAllocation (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Expression> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}
};

struct VarReference : VariableBase
{
    VarReference (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Expression> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}
};


// Block

struct Block : ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> ASTList {};
};

} // AST
} // Compiler


#endif

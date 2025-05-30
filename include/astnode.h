#ifndef ASTNODE_H
#define ASTNODE_H

#include <memory>
#include <string>
#include <vector>


namespace Compiler {
namespace AST {

enum class NodeType {
    Unknown,
    Block,
    VarDeclaration,
    VarDefinition,
    VarAllocation,
    VarReference,
    Expression,
    // ...
};

struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual NodeType getType() { return NodeType::Unknown; };
};

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


    NodeType getType() override { return NodeType::Unknown; };
};

struct VarDeclaration : VariableBase
{
    VarDeclaration(const std::string& name, bool isRuntime)
        : VariableBase(std::move(name), isRuntime, true) {}

    NodeType getType() override { return NodeType::VarDeclaration; };
};

struct VarDefinition : VariableBase
{
    VarDefinition (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Expression> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}

    NodeType getType() override { return NodeType::VarDefinition; };
};

struct VarAllocation : VariableBase
{
    VarAllocation (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Expression> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}

    NodeType getType() override { return NodeType::VarAllocation; };
};

struct VarReference : VariableBase
{
    VarReference (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Expression> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}

    NodeType getType() override { return NodeType::VarReference; };
};


// Block

struct Block : ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> ASTList {};

    NodeType getType() override { return NodeType::Block; };
};

} // AST
} // Compiler


#endif

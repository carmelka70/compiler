#ifndef ASTNODE_H
#define ASTNODE_H

#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace Compiler {
namespace AST {

enum class NodeType {
    Unknown,
    Empty,
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

struct Empty : ASTNode
{
    NodeType getType() override { return NodeType::Empty; };
};


struct Rvalue : ASTNode {};

struct Expression : Rvalue {};

using Int_t = long long;
using Double_t = double;
using Char_t = char;
using String_t = std::string;
using Identifier_t = String_t;

using Literal_t = std::variant<std::monostate,Int_t,Double_t,Char_t,String_t>;

struct Value : ASTNode {};

struct Literal : Value 
{
    Literal_t value;
};

struct Lvalue : Value
{
    Identifier_t identifier;
};
struct Set : Rvalue
{
    std::vector<Literal_t> elements;
    bool isSetValue;
};

// Expression

// Variables

struct VariableBase : ASTNode
{
    const std::string name;
    bool isRuntime;
    bool isDecleration;
    std::unique_ptr<Rvalue> value; // nullptr for declaration

    VariableBase(const std::string& name, bool isRuntime, bool isDecleration, std::unique_ptr<Rvalue> value = nullptr)
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
    VarDefinition (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Rvalue> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}

    NodeType getType() override { return NodeType::VarDefinition; };
};

struct VarAllocation : VariableBase
{
    VarAllocation (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Rvalue> value)
        : VariableBase(std::move(name), isRuntime, isDecleration ,std::move(value)) {}

    NodeType getType() override { return NodeType::VarAllocation; };
};

struct VarReference : VariableBase
{
    VarReference (const std::string& name, bool isRuntime ,bool isDecleration ,std::unique_ptr<Rvalue> value)
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

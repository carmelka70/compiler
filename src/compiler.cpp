#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

#include "compiler.h"

using namespace Compiler;

void Compiler::log(std::string_view msg)
{
    std::cout << msg << '\n';
}

CompileContext Compiler::generateCompilerContext(int argc ,const char *argv[])
{
    if (argc < 2)
    {
        log("ERROR: run compiler src out");
        exit(EXIT_FAILURE);
    }

    CompileContext context {};

    while (argc > 1)
    {
        if (*argv[argc-1] != '-')
            context.sourceFiles.push_back(argv[argc-1]);
        argc--;
    }
    return context;
}

std::string Compiler::getTokenKey(TokenType type)
{
    auto it = std::find_if(kOperators.begin(), kOperators.end(),
        [type](const auto& pair) { return pair.second == type; });

    if (it != kOperators.end())
        return std::string(it->first);

    it = std::find_if(kKeywords.begin(), kKeywords.end(),
        [type](const auto& pair) { return pair.second == type; });

    if (it != kKeywords.end())
        return std::string(it->first);

    switch (type)
    {
        case TokenType::Identifier:
            return "Identifier";
        case TokenType::String:
            return "String";
        case TokenType::Char:
            return "Char";
        case TokenType::Integer:
            return "Integer";
        case TokenType::Double:
            return "Double";
        default: 
            return "Unknown";
    }
}


std::string Compiler::getTokenPos(Token token)
{
    return std::to_string(token.line) + ":" + std::to_string(token.column);
}

void Compiler::printASTNode(const std::unique_ptr<AST::ASTNode>& node) {
    if (node == nullptr)
    {
        log("fail");
        return;
    }
    using namespace AST;

    switch (node->getType()) {
        case NodeType::VarDeclaration: {
            auto* decl = static_cast<const VarDeclaration*>(node.get());
            std::cout << "VarDeclaration: name = " << decl->name
                      << ", runtime = " << decl->isRuntime << "\n";
            break;
        }
        case NodeType::VarDefinition: {
            auto* def = static_cast<const VarDefinition*>(node.get());
            std::cout << "VarDefinition: name = " << def->name
                      << ", runtime = " << def->isRuntime
                      << ", has value = " << (def->value != nullptr) << "\n";
            break;
        }
        case NodeType::VarAllocation: {
            auto* alloc = static_cast<const VarAllocation*>(node.get());
            std::cout << "VarAllocation: name = " << alloc->name
                      << ", runtime = " << alloc->isRuntime << "\n";
            break;
        }
        case NodeType::VarReference: {
            auto* ref = static_cast<const VarReference*>(node.get());
            std::cout << "VarReference: name = " << ref->name
                      << ", runtime = " << ref->isRuntime << "\n";
            break;
        }
        case NodeType::Block: {
            auto* block = static_cast<const Block*>(node.get());
            std::cout << "Block with " << block->ASTList.size() << " children\n";
            for (const auto& stmt : block->ASTList)
                printASTNode(stmt);
            break;
        }
        default:
            std::cout << "Unknown node\n";
    }
}


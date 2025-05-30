#ifndef COMPILER_H
#define COMPILER_H

#include "vector"
#include "string"


namespace Compiler {

void log(std::string_view msg);

struct CompileContext
{
    std::vector<std::string> sourceFiles {};
    std::string outputName {"out"};
    // ...
};

CompileContext generateCompilerContext(int argc ,const char *argv[]);

}; // Compiler

#endif

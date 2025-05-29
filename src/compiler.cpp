#include <iostream>
#include <vector>
#include <cassert>

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

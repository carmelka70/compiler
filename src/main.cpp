#include "compiler.h"
#include "lexer.h"

int main(int argc ,const char *argv[])
{
    auto context = Compiler::generateCompilerContext(argc ,argv);

    Compiler::Lexer lexer(context);

    while (auto token = lexer.getNextToken())
    {
        if (token)
            lexer.printTokenKey(token.value());
    }
}

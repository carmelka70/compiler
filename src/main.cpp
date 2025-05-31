#include "compiler.h"
#include "lexer.h"
#include "parser.h"

int main(int argc ,const char *argv[])
{
    auto context = Compiler::generateCompilerContext(argc ,argv);

    Compiler::Lexer lexer(context);
    Compiler::Parser parser(context);

    while (auto optToken = lexer.getNextToken())
    {
        const auto& token = *optToken;

        parser.consume(token);
        if (parser.statementReady())
        {
            auto node = parser.parse();
            Compiler::printASTNode(node);
        }
    }
    Compiler::log("end");

    // err if parser not empty
}

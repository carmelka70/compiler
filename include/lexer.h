#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <string>
#include <optional>
#include <string_view>

#include "compiler.h"
#include "token.h"

namespace Compiler {
class Lexer
{
    public:
        Lexer(const CompileContext& context);
        ~Lexer() = default;

        std::optional<Token> getNextToken();
        std::optional<Token> peekNextToken();

    private:
        std::ifstream ifs {};
        std::string line {};
        std::optional<Token> nextToken;

        size_t atLine = 0;
        size_t atColumn = 0;

        Token lexIdentifierOrToken(const std::string_view view);

        Token lexNumber(const std::string_view view);

        std::string escapeString(std::string_view input);
        Token lexCharOrString(const std::string_view view);
        Token lexString(const std::string_view view);

        Token lexOperator(const std::string_view view);

        std::optional<Token> tokenizeAtPosition();
        void advancePosition(const std::optional<Token> token);
};

}; //Compiler

#endif

#include <algorithm>
#include <cctype>
#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <fstream>

#include "compiler.h"
#include "lexer.h"

using namespace Compiler;

Lexer::Lexer(const CompileContext& context)
{
    if (context.sourceFiles.empty())
    {
        log("ERROR: No source files provided.");
        exit(EXIT_FAILURE);
    }

    ifs.open(context.sourceFiles[0]);
    if (!ifs.is_open())
    {
        log("ERROR: Failed to open source file.");
        exit(EXIT_FAILURE);
    }

    for (const auto& file : context.sourceFiles)
        std::cout << "Source file: " << file << '\n';
}

template<class T = std::string>
static void keyprint(const std::string_view type ,T value = "None")
{
    std::cout << type << " : " << value << '\n';
}

void Lexer::printTokenKey(Token token)
{
    std::cout << token.line << ',' << token.column << " | ";
    TokenType type = token.type;
    auto value = token.value;
    auto it = std::find_if(kOperators.begin(), kOperators.end(),
        [type](const auto& pair) { return pair.second == type; });

    if (it != kOperators.end())
    {
        keyprint(it->first);
        return;
    }

    it = std::find_if(kKeywords.begin(), kKeywords.end(),
        [type](const auto& pair) { return pair.second == type; });

    if (it != kKeywords.end())
    {
        keyprint(it->first);
        return;
    }

    switch (type)
    {
        case TokenType::Identifier:
            keyprint("Identifier" ,std::get<Identifier_t>(value));
            break;
        case TokenType::String:
            keyprint("String" ,std::get<String_t>(value));
            break;
        case TokenType::Char:
            keyprint("Char" ,std::get<Char_t>(value));
            break;
        case TokenType::Integer:
            keyprint("Integer" ,std::get<Int_t>(value));
            break;
        case TokenType::Double:
            keyprint("Double" ,std::get<Double_t>(value));
            break;
        default: keyprint("Unknown");
    }
}

std::optional<Token> Lexer::lexIdentifierOrToken(const std::string_view view)
{
    auto it = std::find_if_not(view.begin() ,view.end()
            ,[](char c) {return std::isalnum(c) || c == '_';});
    size_t count = static_cast<size_t>(it - view.begin());

    atColumn += count;

    std::string_view substr(view.data(), count);
    auto type = kKeywords.find(substr);

    if (type == kKeywords.end())
    {
        return Token{TokenType::Identifier, std::string(view.substr(0 ,count)), atLine, atColumn};
    }

    return Token{type->second, std::monostate(), atLine, atColumn};
}

size_t countDigits(std::string_view view, size_t idx) {
    auto it = std::find_if_not(view.begin() + idx ,view.end() ,::isdigit);
    return static_cast<size_t>(it - (view.begin() + idx));
}

std::optional<Token> Lexer::lexNumber(const std::string_view view)
{
    size_t whole_length = countDigits(view ,0);

    if (whole_length < view.size() && view[whole_length] == '.') // if double
    {
        size_t fraction_length = countDigits(view ,whole_length + 1);

        double d;
        if (std::from_chars(view.data(), view.data() + (whole_length + 1 + fraction_length), d).ec != std::errc()) {
            std::cout << "failed to read number \'" << view << "\' \n"; 
            return std::nullopt;
        }

        atColumn += whole_length + 1 + fraction_length;
        return Token{TokenType::Double ,d ,atLine, atColumn};
    }
    else // if int
    {
        long l;
        if (std::from_chars(view.data(), view.data() + whole_length ,l).ec != std::errc()) {
            //LOG
            std::cout << "failed to read number \'" << view << "\' \n"; 
            return std::nullopt;
        }

        atColumn += whole_length;
        return Token{TokenType::Integer ,l ,atLine, atColumn};
    }

}

std::optional<std::string> escapeString(std::string_view input)
{
    std::string escapedStr {};
    escapedStr.reserve(input.length());

    enum {CHAR ,ESCAPE} state = CHAR;

    for (int i = 0; i < input.size(); i++)
    {
        switch (state)
        {
            case CHAR:
                if (input[i] == '\\')
                    state = ESCAPE;
                else
                    escapedStr.push_back(input[i]);
                break;
            case ESCAPE:
                switch (input[i])
                {
                    case 'n': escapedStr.push_back('\n'); break;
                    case 't': escapedStr.push_back('\\'); break;
                    case '\\': escapedStr.push_back('\\'); break;
                    case '\'': escapedStr.push_back('\''); break;
                    case '\"': escapedStr.push_back('\"'); break;

                    default: return std::nullopt;
                }
                state = CHAR;
        }
    }
    return escapedStr;
}

std::optional<Token> Lexer::lexCharOrString(const std::string_view view)
{
    size_t count = 1;
    while (count < view.size())
    {
        if (view[count] == '\'' && view[count - 1] != '\\') {
            ++count;
            break;
        }
        ++count;
    }

    atColumn += count;

    if (count > view.size() || view[count - 1] != '\'') {
        return std::nullopt;
    }

    if (count == 0)
        return Token{TokenType::Char ,'\0' ,atLine ,atColumn};

    if (count < 3)
    {
        // LOG
    }
    auto opt = escapeString(view.substr(1 ,count - 2));
    if (!opt)
        return std::nullopt;

    std::string escapedStr = *opt;

    if (escapedStr.size() == 1) // if char
        return Token{TokenType::Char ,escapedStr[0] ,atLine ,atColumn};
    else // if string
        return Token{TokenType::String ,escapedStr ,atLine ,atColumn};
}


std::optional<Token> Lexer::lexString(const std::string_view view)
{
    size_t count = 1;
    while (count < view.size())
    {
        if (view[count] == '\"' && view[count - 1] != '\\') {
            ++count;
            break;
        }
        ++count;
    }

    atColumn += count;

    if (count > view.size() || view[count - 1] != '\"') {
        return std::nullopt;
    }

    if (count == 0)
        return Token{TokenType::String ,"" ,atLine ,atColumn};

    auto opt = escapeString(view.substr(1 ,count - 2));
    if (!opt)
        return std::nullopt;

    std::string escapedStr = *opt;

    return Token{TokenType::String ,escapedStr ,atLine ,atColumn};
}

std::optional<Token> Lexer::lexOperator(const std::string_view view)
{
    auto it = std::find_if_not(view.begin() ,view.end()
            ,[](char c) {return !std::isspace(c);});
    size_t count = static_cast<size_t>(it - view.begin());

    std::string_view substr(view.data(), count);
    while(count > 0)
    {
        auto type = kOperators.find(substr);

        if (type != kOperators.end())
        {
            atColumn += count;
            return Token{type->second, std::monostate(), atLine, atColumn};
        }
        count--;
        substr.remove_suffix(1);
    }
    return std::nullopt;
}


std::optional<Token> Lexer::tokenizeAtPosition()
{
    // skip whitespaces
    while (atColumn < line.size() && std::isspace(line[atColumn]))
        atColumn++;

    if (atColumn >= line.size())
    {
        if (!std::getline(ifs ,line))
            return std::nullopt;
        atLine++;
        atColumn = 0;
        return tokenizeAtPosition();
    }

    // determine token type

    const std::string_view view {line.data() + atColumn};
    const char ch = view[0];
    if (std::isalpha(ch) || ch == '_')
        return lexIdentifierOrToken(view);
    else if (std::isdigit(ch))
        return lexNumber(view);
    else if (ch == '\'')
        return lexCharOrString(view);
    else if (ch == '\"')
        return lexString(view);
    else
        return lexOperator(view);
}


std::optional<Token> Lexer::getNextToken()
{
    if (nextToken)
    {
        auto token = nextToken;
        nextToken.reset();
        return token;
    }

    return tokenizeAtPosition();
}
std::optional<Token> Lexer::peekNextToken()
{
    if (!nextToken)
    {
        auto tempLine = line;
        auto tempAtColumn = atColumn;
        auto tempAtLine = atLine;

        nextToken = getNextToken();

        line = tempLine;
        atColumn = tempAtColumn;
        atLine = tempAtLine;
    }
    return nextToken;
}

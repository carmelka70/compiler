#include <algorithm>
#include <cctype>
#include <charconv>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <fstream>

#include "compiler.h"
#include "lexer.h"

using namespace Compiler;

Lexer::Lexer(const CompileContext& context)
{
    if (context.sourceFiles.empty())
        throw std::runtime_error("ERROR: No source files provided.");

    ifs.open(context.sourceFiles[0]); //tbd
    if (!ifs.is_open())
        throw std::runtime_error("ERROR: Failed to open source file.");
}

template<class T = std::string>
static void keyprint(const std::string_view type ,T value = "None")
{
    std::cout << type << " : " << value << '\n';
}

void Lexer::printTokenKey(Token token)
{
#ifndef NDEBUG
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
#endif
}

Token Lexer::lexIdentifierOrToken(const std::string_view view)
{
    auto it = std::find_if_not(view.begin() ,view.end()
            ,[](char c) {return std::isalnum(c) || c == '_';});
    size_t count = static_cast<size_t>(it - view.begin());

    atColumn += count;

    std::string_view substr = view.substr(0 ,count);
    auto type = kKeywords.find(substr);

    if (type == kKeywords.end()) // identifier
        return Token{TokenType::Identifier, std::string(substr), atLine, atColumn};

    return Token{type->second, std::monostate(), atLine, atColumn};
}

size_t countDigits(std::string_view view, size_t idx) {
    auto it = std::find_if_not(view.begin() + idx ,view.end() ,::isdigit);
    return static_cast<size_t>(it - (view.begin() + idx));
}

Token Lexer::lexNumber(const std::string_view view)
{
    size_t whole_length = countDigits(view ,0);

    if (whole_length < view.size() && view[whole_length] == '.') // if double
    {
        size_t fraction_length = countDigits(view ,whole_length + 1);

        double d;
        if (std::from_chars(view.data(), view.data() + (whole_length + 1 + fraction_length), d).ec != std::errc())
            throw std::runtime_error("FATAL: failed to read double: \'" + std::string(view.data() ,whole_length + 1 + fraction_length) + "\' at line " + std::to_string(atLine) + ".");

        atColumn += whole_length + 1 + fraction_length;
        return Token{TokenType::Double ,d ,atLine, atColumn};
    }
    else // if int
    {
        Int_t i;
        if (std::from_chars(view.data(), view.data() + whole_length ,i).ec != std::errc())
            throw std::runtime_error("FATAL: failed to read int: \'" + std::string(view.data() ,whole_length) + "\' at line " + std::to_string(atLine) + ".");

        atColumn += whole_length;
        return Token{TokenType::Integer ,i ,atLine, atColumn};
    }
}

std::string Lexer::escapeString(std::string_view input)
{
    std::string escapedStr;
    escapedStr.reserve(input.length());

    enum {CHAR ,ESCAPE} state = CHAR;

    for (char c : input)
    {
        switch (state)
        {
            case CHAR:
                if (c == '\\')
                    state = ESCAPE;
                else
                    escapedStr.push_back(c);
                break;
            case ESCAPE:
                switch (c)
                {
                    case 'a': escapedStr.push_back('\a'); break;
                    case 'b': escapedStr.push_back('\b'); break;
                    //case 'e': escapedStr.push_back('\e'); break;
                    case 'f': escapedStr.push_back('\f'); break;
                    case 'n': escapedStr.push_back('\n'); break;
                    case 'r': escapedStr.push_back('\r'); break;
                    case 't': escapedStr.push_back('\t'); break;
                    case 'v': escapedStr.push_back('\v'); break;
                    case '\\': escapedStr.push_back('\\'); break;
                    case '\'': escapedStr.push_back('\''); break;
                    case '\"': escapedStr.push_back('\"'); break;
                    case '\?': escapedStr.push_back('\?'); break;

                    default: // unknown escape sequence
                        throw std::runtime_error("FATAL: unusable escape sequence at line " + std::to_string(atLine) + ": \'\\" + c + "\'.");
                }
                state = CHAR;
        }
    }
    return escapedStr;
}

Token Lexer::lexCharOrString(const std::string_view view)
{
    size_t count = 1;
    while (count < view.size())
    {
        if (view[count] == '\'' && view[count - 1] != '\\') { // dont exit on \'
            ++count;
            break;
        }
        ++count;
    }

    atColumn += count;

    if (count > view.size() || view[count - 1] != '\'')
        throw std::runtime_error("FATAL: string at line " + std::to_string(atLine) + " isnt closed properly.");

    if (count == 0) // empty char ('')
        return Token{TokenType::Char ,'\0' ,atLine ,atColumn};



    std::string escapedStr = escapeString(view.substr(1 ,count - 2));

    if (escapedStr.size() == 1) // if char
        return Token{TokenType::Char ,escapedStr[0] ,atLine ,atColumn};
    else // if string
        return Token{TokenType::String ,escapedStr ,atLine ,atColumn};
}


Token Lexer::lexString(const std::string_view view)
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

    if (count > view.size() || view[count - 1] != '\"')
        throw std::runtime_error("FATAL: string at line " + std::to_string(atLine) + " isnt closed properly.");

    if (count == 0) // empty string ("")
        return Token{TokenType::String ,"" ,atLine ,atColumn};



    std::string escapedStr = escapeString(view.substr(1 ,count - 2));
    return Token{TokenType::String ,escapedStr ,atLine ,atColumn};
}

Token Lexer::lexOperator(const std::string_view view)
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
    throw std::runtime_error("FATAL: unintelligible gibberish at line " + std::to_string(atLine) + ".");
}


std::optional<Token> Lexer::tokenizeAtPosition()
{
    // skip whitespaces
    while (atColumn < line.size() && std::isspace(line[atColumn]))
        atColumn++;

    if (atColumn >= line.size())
    {
        if (!std::getline(ifs ,line))
            return std::nullopt; // EOF
        atLine++;
        atColumn = 0;
        return tokenizeAtPosition();
    }

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

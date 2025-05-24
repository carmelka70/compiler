#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

enum class TokenType
{
    Unknown = 0,
    Keyword,
    Identifier,
    Integer,
    Double,
    Char,
    String,
    Define,
    New,
    Create,
    Delete,
};

struct Token
{
    TokenType type {TokenType::Unknown};
    std::variant<
        std::monostate,
        long,
        double,
        char,
        std::string_view
            > Data {};
    size_t line {};
    size_t row {};

};

class Lexer
{
    public:
        Lexer();
        ~Lexer();

        std::optional<Token> getNextToken();
        std::optional<Token> peekNextToken();

    private:
        std::ifstream ifs {};
        std::string line {};
        size_t position = 0;
        std::optional<Token> nextToken;

        size_t atLine = 0;
        size_t atColumn = 0;

        const std::unordered_map<std::string_view ,TokenType> kKeywords =
        {
            {"define", TokenType::Define},
            {"new", TokenType::New},
            {"create", TokenType::Create},
            {"delete", TokenType::Delete},
        };

        const std::unordered_map<std::string_view ,TokenType> kOperators =
        {
            {"=", TokenType::Define},
            {":", TokenType::New},
            {":=", TokenType::Create},
            {"delete", TokenType::Delete},
        };

        std::optional<Token> lexIdentifierOrToken(const std::string_view view);
        std::optional<Token> lexNumber(const std::string_view view);
        std::optional<Token> lexCharOrString(const std::string_view view);
        std::optional<Token> lexString(const std::string_view view);
        std::optional<Token> lexOperator(const std::string_view view);

        std::optional<Token> tokenizeAtPosition();
        void advancePosition(const std::optional<Token> token);
};

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
        return Token{TokenType::Identifier, view.substr(0 ,count), atLine, atColumn-1};
    }

    return Token{type->second, std::monostate(), atLine, atColumn-1};
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
        if (std::from_chars(view.data(), view.data() + (whole_length + 1 + fraction_length), d).ec == std::errc()) {
            //LOG
            return std::nullopt;
        }

        atColumn += whole_length + 1 + fraction_length;
        return Token{TokenType::Double ,d ,atLine, atColumn-1};
    }
    else // if int
    {
        long l;
        if (std::from_chars(view.data(), view.data() + whole_length ,l).ec == std::errc()) {
            //LOG
            return std::nullopt;
        }

        atColumn += whole_length;
        return Token{TokenType::Integer ,l ,atLine, atColumn-1};
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

    if (count >= view.size() || view[count - 1] != '\'') {
        return std::nullopt;
    }

    if (count == 0)
        return Token{TokenType::Char ,'\0' ,atLine ,atColumn-1};

    if (count < 3)
    {
        // LOG
    }
    auto opt = escapeString(view.substr(1 ,count - 2));
    if (!opt)
        return std::nullopt;

    std::string escapedStr = *opt;

    if (escapedStr.size() == 1) // if char
        return Token{TokenType::Char ,escapedStr[0] ,atLine ,atColumn-1};
    else // if string
        return Token{TokenType::String ,escapedStr ,atLine ,atColumn-1};
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

    if (count >= view.size() || view[count - 1] != '\"') {
        return std::nullopt;
    }

    if (count == 0)
        return Token{TokenType::String ,"" ,atLine ,atColumn-1};

    auto opt = escapeString(view.substr(1 ,count - 1));
    if (!opt)
        return std::nullopt;

    std::string escapedStr = *opt;

    return Token{TokenType::String ,escapedStr ,atLine ,atColumn-1};
}

std::optional<Token> Lexer::lexOperator(const std::string_view view)
{
    auto it = std::find_if_not(view.begin() ,view.end()
            ,[](char c) {return std::isalnum(c) || c == '_';});
    size_t count = static_cast<size_t>(it - view.begin());

    atColumn += count;

    std::string_view substr(view.data(), count);
    auto type = kKeywords.find(substr);

    if (type == kKeywords.end())
    {
        return Token{TokenType::Identifier, view.substr(0 ,count), atLine, atColumn-1};
    }

    return Token{type->second, std::monostate(), atLine, atColumn-1};
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

#include <cctype>
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
    unsigned line {};
    unsigned row {};

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
        std::string line = "";
        unsigned atLine = 0;
        unsigned atColumn = 0;

        const std::unordered_map<std::string_view ,TokenType> kKeywords =
        {
            {"define", TokenType::Define},
            {"new", TokenType::New},
            {"create", TokenType::Create},
            {"delete", TokenType::Delete},
        };

        std::optional<Token> lexIdentifierOrToken();
        std::optional<Token> lexIdentifier(int length);
        std::optional<Token> lexNumber();
        std::optional<Token> lexInt(int length);
        std::optional<Token> lexDouble(int length);
        std::optional<Token> lexCharOrString();
        std::optional<Token> lexString();
        std::optional<Token> lexOperator();
};

std::optional<Token> Lexer::lexIdentifierOrToken()
{
    size_t count;
    for (count = 1; count < line.size() &&
            (std::isalnum(line[count]) || line[count] == '_' );
            count++)
        ;
    atColumn += count;

    std::string_view substr(line.data(), count);
    auto type = kKeywords.find(substr);

    if (type == kKeywords.end())
    {
        return Token{TokenType::Identifier, line.substr(0 ,count), atLine-1, atColumn-1};
    }

    return Token{type->second, std::monostate(), atLine-1, atColumn-1};
}

size_t countDigits(std::string_view line, size_t idx) {
    size_t count;
    for (count = 1; idx + count < line.size() &&
            isdigit(line[idx + count]);
            count++)
        ;
    return count;
}

std::optional<Token> Lexer::lexNumber()
{
    size_t whole_length = countDigits(line ,0);
    atColumn += whole_length;

    if (whole_length < line.size() && line[whole_length] == '.') // if double
    {
        size_t fraction_length = countDigits(line ,whole_length + 1);
        atColumn += 1 + fraction_length;

        return Token{TokenType::Double
            ,std::stod(line.substr(0 ,1 + whole_length + fraction_length)) 
            ,atLine-1, atColumn-1};
    }
    else // if int
        return Token{TokenType::Integer 
            ,std::stol(line.substr(0 ,whole_length)) 
            ,atLine-1, atColumn-1};
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
        }
    }
    return escapedStr;
}

std::optional<Token> Lexer::lexCharOrString()
{
    size_t count;
    for (count = 1; count < line.size() &&
            line[count-1] != '\\' &&
            line[count] == '\'';
            count++)
        ;
    std::string inner = line.substr(1 ,count - 1);
    auto opt = escapeString(inner);
    if (!opt)
        return std::nullopt;

    std::string escapedStr = *opt;

    if (escapedStr.size() == 1) // if char
        return Token{TokenType::Char ,escapedStr[0] ,atLine-1 ,atColumn-1};
    else // if string
        return Token{TokenType::String ,escapedStr ,atLine-1 ,atColumn-1};
}


std::optional<Token> Lexer::lexString()
{
    size_t count;
    for (count = 1; count < line.size() &&
            line[count-1] != '\\' &&
            line[count] == '\"';
            count++)
        ;
    std::string inner = line.substr(1 ,count - 1);
    auto opt = escapeString(inner);
    if (!opt)
        return std::nullopt;

    std::string escapedStr = *opt;

    return Token{TokenType::String ,escapedStr ,atLine-1 ,atColumn-1};
}


std::optional<Token> Lexer::getNextToken()
{

    if (line.empty())
    {
        std::getline(ifs ,line);
        atLine++;
    }

    // skip whitespaces
    int i;
    for (i = 0; i < line.size() && std::isspace(line[i]); i++)
        ;
    line.erase(0 ,i);
    atColumn += i;


    // determine token type

    char ch = line[0];
    if (std::isalpha(ch) || ch == '_')
        return lexIdentifierOrToken();
    else if (std::isdigit(ch))
        return lexNumber();
    else if (ch == '\'')
        return lexCharOrString();
    else if (ch == '\"')
        return lexString();
    else
        return lexOperator();
}

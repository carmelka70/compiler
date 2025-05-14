#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


namespace Compiler
{
    static void log(std::string_view);

    struct CompileContext
    {
        std::vector<std::string> sourceFiles {};
        std::string outputName {"out"};
        // ...
    };

    struct CompileContext
    generateCompilerContext(int argc ,const char *argv[]);


    struct ParserOutput
    {
        // ...
    };

    class Assembler
    {
        public:
            Assembler(const CompileContext& context);
            ~Assembler() = default;

            void collectAST(const ParserOutput& in);
            void checkSyntax();
            void generateIR();
            void optimize();
            void assemble();
            void output();
    };


    enum class TokenType
    {
        kUnknown = 0,
        /* keywords */
        kIf,
        kWhile,
        kFor,
        kDefine,
        /* operators */
        kAssign,
        kReference,
        kDeclare,
        kEquals,
        // ...
    };

    struct LexerOutput
    {
        // ...
    };

    class Parser
    {
        public:
            Parser(const CompileContext& context);
            ~Parser() = default;


            void collectTokens(const LexerOutput& in);
            void checkSyntax();
            void optimize();
            void buildAST();

            ParserOutput getAST();
    };


    class Lexer
    {
        public:
            Lexer(const CompileContext& context);
            ~Lexer() = default;

            void tokenize();
            LexerOutput getTokens();

        private:
            using TokenVarient_t = std::variant<
                std::monostate,
                long,
                double,
                bool,
                std::string>;

            struct Token
            {
                TokenVarient_t data {};
                TokenType type = TokenType::kUnknown;
                unsigned line = 0;
                unsigned column = 0;
            };

            std::vector<Token> tokenStream_;
            std::string filename_;

            static inline const std::unordered_map<std::string_view ,TokenType> kKeywords_ {
                    {"if", TokenType::kIf},
                    {"while", TokenType::kWhile},
                    {"for", TokenType::kFor},
                    {"define", TokenType::kDefine},
            };

            static inline const std::unordered_map<std::string_view ,TokenType> kOperators_ {
                    {"=", TokenType::kAssign},
                    {":=", TokenType::kReference},
                    {":", TokenType::kDeclare},
                    {"==", TokenType::kEquals},
            };

    };

    class App
    {
        public:
            void run(int argc ,const char *argv[]);
    };


    // Lexer -> Parser -> Assembler --> output
    void App::run(int argc ,const char *argv[])
    {
        auto context = Compiler::generateCompilerContext(argc ,argv);

        Lexer lexer(context);
        Parser parser(context);
        Assembler assembler(context);

        lexer.tokenize();

        parser.collectTokens(lexer.getTokens());
        parser.checkSyntax();
        parser.optimize();
        parser.buildAST();

        assembler.collectAST(parser.getAST());
        assembler.checkSyntax();
        assembler.optimize();
        assembler.generateIR();
        assembler.output();
    }

    int main(int argc ,const char *argv[])
    {
        App app;
        app.run(argc ,argv);
    }
}

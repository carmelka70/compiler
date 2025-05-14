int main(int argc ,const char *argv[])
{
    auto context = compiler::generateCompilerContext(argc ,argv);

    Lexer lexer(context);
    Parser parser(context);
    Assembler assembler(context);

    lexer.tokenize();
    
    parser.collect(lexer.getTokens());
    parser.checkForSyntaticErrors();
    parser.preparseOptimize();
    parser.parse();

    assembler.collect(parser.getAST());
    assembler.checkForSyntaxErrors();
    assembler.optimize();
    assembler.assemble();
    assembler.output();
}

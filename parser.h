#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "variable.h"


class Parser{
    public:
    std::vector<Token> tokenList;
    VariableList symbolTable;
    Token token;
    std::string output;

    Parser();
    Token Peek(int);

    Token expect(TokenType);
    void parseProgram();
    void parseGlobalVars();
    void parseVarDeclList();
    void parseVarList(VariableType);
    void parseBody();
    void parseStmtList();
    void parseAssignment();
    VariableType parseExpression();
    void parseIf();
    void parseWhile();
    void parseSwitch();
    void printTokenList();

    private:
    int index; //how far along in the token list we are

    void SyntaxError();
};

#endif
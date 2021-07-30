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
    void parseVarList();
    void parseScope();
    void parsePublicVars();
    void parsePrivateVars();
    void parseStmtList();
    void parseStmt();
    void printTokenList();

    private:
    int index; //how far along in the token list we are
    bool currentlyPublic;
    std::vector<std::string> scopeList;

    void SyntaxError();
};

#endif
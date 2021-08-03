#include <vector>
#include <iostream>

#include "lexer.h"
#include "parser.h"
#include "variable.h"

using namespace std;



Parser::Parser(){
    index = 0;
    output = "";
}


// peek requires that the argument "howFar" be positive.
Token Parser::Peek(int howFar)
{
    if (howFar <= 0) {      // peeking backward or in place is not allowed
        cout << "LexicalAnalyzer:peek:Error: non positive argument\n";
        exit(-1);
    } 

    int peekIndex = index + howFar - 1;
    if (peekIndex > (int)(tokenList.size()-1)) { // if peeking too far
        Token token;                        // return END_OF_FILE
        token.lexeme = "";
        token.line_no = -1;
        token.token_type = END_OF_FILE;
        return token;
    } else{
        return tokenList[peekIndex];
        
    }

        
}

void Parser::SyntaxError()
{
    cout << "Syntax Error\n";
    exit(1);
}

void Parser::printTokenList(){
    for(auto elem : tokenList){
        elem.Print();
    }
    return;
}

//increments the index of tokenList in the function itself
Token Parser::expect(TokenType expected_type)
{
    Token t = tokenList[index];
    if(t.token_type != expected_type){
        SyntaxError();
    }
    //we've now made sense of this symbol
    index++;
    return t;
}


void Parser::parseProgram(){
    //will either start with global variables or the body
    //if global variables, we have a variable declaration - ID and comma or :
    //if body, it'll start with a lbrace
    token = tokenList[index];
    if(token.token_type == ID){
        token = Peek(2);
        if(token.token_type == COMMA || token.token_type == COLON){
            //this is a global var list
            //parse global vars
            parseGlobalVars();
        }
        else{
            SyntaxError();
        }
    }
    //cool now we're past the global variables let's look for the body
    token = tokenList[index];
    if(token.token_type == LBRACE){
        parseBody();
    }
    else{
        SyntaxError();
    }
    //if we're here, then we've got an end of file, hopefully
    token = tokenList[index];
    if(token.token_type == END_OF_FILE){
        return;
    }
    else{
        SyntaxError();
    }
}

void Parser::parseGlobalVars(){
    VariableType currentType;
    //variables are defined in here so we'll be working closely with the var class
    //first we need to see what type this line of variables will be
    int i = 1;
    while(Peek(i).token_type != COLON){
        i++;
    }
    //we have the colon, one more will be the variable type
    //TODO: make sure this works
    token = Peek(i+1);
    if(token.token_type != ID){
        SyntaxError();
    }
    else{
        if(token.lexeme == "int"){
            currentType = INT;
        }
        else if(token.lexeme == "real"){
            currentType = REAL;
        }
        else if(token.lexeme == "bool"){
            currentType == BOOL;
        }
    }
    //we have the variable type now we can make the symbol table of variables here
    


}

void Parser::parseVarDeclList(){
    //can either be a single declaration or a list of them
    //will always start with a single declaration, or rather an ID

}

void Parser::parseBody(){

}


int main()
{
    LexicalAnalyzer lexer;
    Parser* parser = new Parser();
    Token token;

    token = lexer.GetToken();
    if(token.token_type != ERROR){  //is this cheating? yup 100%
         parser->tokenList.push_back(token); //add what we just determined to the end of the token list
    }

    //find the list of tokens
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        if(token.token_type != ERROR){  //is this cheating? yup 100%
             parser->tokenList.push_back(token); //add what we just determined to the end of the token list
        }
    }

    parser->printTokenList();

    //parse that list into actual output
    //parser->parseProgram();
    //cout << parser->output;
    return 1;
}

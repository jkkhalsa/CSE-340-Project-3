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
    //variables are defined in here so we'll be working closely with the var class
    //we have the variable type now we can make the symbol table of variables here
    parseVarDeclList();
    return;
}

void Parser::parseVarDeclList(){
    //can either be a single declaration or a list of them
    //will always start with a single declaration, or rather an ID
    //first we need to see what type this line of variables will be
    VariableType currentType;
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
            currentType = BOOL;
        }
    }
    parseVarList(currentType);
    expect(COLON);
    expect(ID);
    expect(SEMICOLON);

    //now that we've parsed this list, we either have another list or the body
    //check
    if(Peek(1).token_type == LBRACE){
        parseBody();
    }
    else if(Peek(1).token_type == ID){
        parseVarDeclList();
    }
    else{
        SyntaxError();
    }

}

void Parser::parseVarList(VariableType currentType){
    //a var list goes ID-COMMA-ID-COMMA until it hits a colon
    token = tokenList[index];
    if(tokenList[index].token_type != ID){
        SyntaxError();
    }
    else{
        //it's an ID, so a variable
        symbolTable.addKnownVariable(token.lexeme, currentType);
        //we've now made sense of this token
        index++;
        expect(COMMA);
    }
    token = Peek(1);
    if(token.token_type == COLON){
        //going back up to the var declaration list
        return;
    }
    else if(token.token_type == ID){
        parseVarList(currentType);
    }
    else{
        SyntaxError();
    }
}

void Parser::parseBody(){
    //we should start with a lbrace
    expect(LBRACE);
    //good now that's over with
    //now statements wheeeeee
    parseStmtList();
    expect(RBRACE);
    return;
}

void Parser::parseStmtList(){
    //we can come into a statement list completely fucked up, so make sure this is one
    //can start with an ID, an IF, a While, or a Switch
    token = tokenList[index];
    if(!(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)){
        SyntaxError();
    }
    //ok good we've confirmed this is a proper statement list
    //check for assignment statement
    if(token.token_type == ID){
        index++;
        parseAssignment();
    }
    else if(token.token_type == IF){
        index++;
        parseIf();
    }
    else if(token.token_type == WHILE){
        index++;
        parseWhile();
    }
    else if(token.token_type == SWITCH){
        index++;
        parseSwitch();
    }
    //statement list will always have either another statement list after it or the end of the body
    //might be a this token instead of a peek
    if(Peek(1).token_type != RBRACE){
        parseStmtList();
    }
    return;
}

void Parser::parseAssignment(){
    Variable leftHand;
    VariableType rightHandType;
    token = tokenList[index];
    //an assignment statement will always be ID equals EXPRESSION ;
    //we already checked if this is an ID before calling this
    //get the left side of the expression into the variable list
    leftHand = symbolTable.searchList(token.lexeme);
    index++;

    expect(EQUAL);

    //now parse the expression on the right hand side
    //passing what's on the left so we can do unknown cleanup in expression
    rightHandType = parseExpression(leftHand);

    //check and make sure we don't have a type error C1
    if(rightHandType != leftHand.type){
        cout << "TYPE MISMATCH " << token.line_no << " C1\n";
        exit(1);
    }
    
    expect(SEMICOLON);
    return;
}


//for expressions where there's a left hand side to be resolved
VariableType Parser::parseExpression(Variable leftHand){
    Variable rightHand;
    VariableType expressionType;

    //these two are for unary and binary type checking
    VariableType rtype1;
    VariableType rtype2;
    Variable rightVar;
    Variable leftVar;
    int lastChecked;
    //whee we're here
    //will either start with an id, num, realnum, binary operator, or unary operator
    //primary first because that just returns right back up after some cleanup
    token = tokenList[index];
    if(token.token_type == ID){
        //this is a variable = variable situation
        //search for the variable
        rightHand = symbolTable.searchList(token.lexeme);
        expressionType = rightHand.type;

        //check for a c1 error just in case
        if(rightHand.type != leftHand.type && rightHand.type != UNKNOWN && leftHand.type != UNKNOWN){
            cout << "TYPE MISMATCH " << token.line_no << " C1\n";
        }

        //now resolve our unknowns
        if(rightHand.type == UNKNOWN){
            symbolTable.resolveUnknownVariables(leftHand.unknownNum, rightHand.unknownNum, leftHand.type);
        }
        else if(leftHand.type == UNKNOWN){
            symbolTable.resolveUnknownVariables(rightHand.unknownNum, leftHand.unknownNum, rightHand.type);
        }
        //we've made sense of this token
        index++;
        return expressionType;
    }
    else if(token.token_type == NUM){
        expressionType = INT;
        //this is a variable = number situation
        //make sure the variable is an int
        if(leftHand.type == UNKNOWN){
            leftHand.type = INT;
            symbolTable.resolveUnknownVariables(leftHand.unknownNum, 0, INT);
            //leftHand.unknownNum = 0;
        }
        if(leftHand.type != INT){
            cout << "TYPE MISMATCH " << token.line_no << " C1\n";
            exit(1);
        }
        //we've made sense of this token
        index++;
        return expressionType;
    }
    else if(token.token_type == REALNUM){
        expressionType = REAL;
        //this is a variable = decimal situation
        //make sure the variable is a real
        if(leftHand.type == UNKNOWN){
            leftHand.type = REAL;
            symbolTable.resolveUnknownVariables(leftHand.unknownNum, 0, REAL);
            //leftHand.unknownNum = 0;
        }
        if(leftHand.type != REAL){
            cout << "TYPE MISMATCH " << token.line_no << " C1\n";
            exit(1);
        }
        //we've made sense of this token
        index++;
        return expressionType;
    }
    else if(token.token_type == TR || token.token_type == FA){
        expressionType = BOOL;
        //this is a boolean value setting situation
        //make sure the variable is a bool
        if(leftHand.type == UNKNOWN){
            leftHand.type = BOOL;
            symbolTable.resolveUnknownVariables(leftHand.unknownNum, 0, BOOL);
            //leftHand.unknownNum = 0;
        }
        if(leftHand.type != BOOL){
            cout << "TYPE MISMATCH " << token.line_no << " C1\n";
        }
        //we've made sense of this token
        index++;
        return expressionType;
    }

    //ok if we're down here then things aren't this simple ughhhhhhh
    //so this is either a binary or unary operator expression
    //unary will always be NOT and should always have type bool
    if(token.token_type == NOT){
        //now made sense of this token
        index++;
        expressionType = BOOL;

        parseUnary();

        //if we come back from parseUnary and there's a left-hand side, then that
        //left hand side must be unknown or BOOL or else there's a c1 mismatch
        if(leftHand.type != BOOL && leftHand.type != UNKNOWN){
            cout << "TYPE MISMATCH " << token.line_no << " C1";
        }
        //if the left hand side is unknown, we can go ahead and change it
        if(leftHand.type == UNKNOWN){
            symbolTable.resolveUnknownVariables(leftHand.unknownNum, 0, BOOL);
        }
        return expressionType;
    }
    else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT ||
    token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS ||
    token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL ||
    token.token_type == NOTEQUAL){
        //WHEW THAT'S A LONG IF
        //anyway this is a binary operator expression
        rtype1 = parseExpression();
        rtype2 = parseExpression();
        //if the two expression types aren't the same, this is a c2 fuckup
        if(rtype1 != rtype2 && rtype1 != UNKNOWN && rtype2 != UNKNOWN){
            cout << "TYPE MISMATCH " << token.line_no << " C2\n";
        }
        //TODO: figure out how to resolve both sides of an unknown expression here to the same thing

    }
    return expressionType;

}


//for expressions where there's no left hand side to resolve
VariableType Parser::parseExpression(){
    Variable rightHand;
    VariableType expressionType;

    VariableType rtype1;
    VariableType rtype2;
    Variable rightVar;
    Variable leftVar;
    int lastChecked;

    //parsing primaries if there's no left side to resolve is very easy
    //will start with id, num, realnum, tr/false if primary
    token = tokenList[index];
    if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA){
        //nest the if so we can increment index and return the type only once
        if(token.token_type == ID){
            //this is a variable
            //search for the variable
            rightHand = symbolTable.searchList(token.lexeme);
            expressionType = rightHand.type;
        }
        else if(token.token_type == NUM){
            expressionType = INT;
        }
        else if(token.token_type == REALNUM){
            expressionType = REAL;
        }
        else if(token.token_type == TR || token.token_type == FA){
            expressionType = BOOL;
        }
        //we've now made sense of this token
        index++;
        return expressionType;
    }

    //if we're down here then we have yet another expression whee
    //will either be a unary or binary expression
    if(token.token_type == NOT){
        //now made sense of this token
        index++;
        expressionType = BOOL;
        parseUnary();

        //no left hand side to be resolved, can just go right back up
        return expressionType;
    }
    else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT ||
    token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS ||
    token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL ||
    token.token_type == NOTEQUAL){
        //WHEW THAT'S A LONG IF
        //anyway this is a binary operator expression
        rtype1 = parseExpression();
        rtype2 = parseExpression();
    }


    
}

void Parser::parseUnary(){
    VariableType rType;
    Variable rightVar;

    token = tokenList[index];
    rType = parseExpression();
    if(rType != BOOL && rType != UNKNOWN){
        cout << "TYPE MISMATCH " << token.line_no << " C3";
        exit(1);
    }
    //if the type is unknown, then we must have returned directly from a primary variable
    //so it's safe to look at the last searched and make it a boolean
    if(rType == UNKNOWN){
        //look for the variable last searched
        rightVar = symbolTable.list[symbolTable.checkedIndex];
        //make sure it's an unknown because I'm paranoid
        if(rightVar.type == UNKNOWN){
            //we can resolve all of this unknown number to a boolean
            symbolTable.resolveUnknownVariables(rightVar.unknownNum, 0, BOOL);
        }
    }
    return;
}

void Parser::parseIf(){
    VariableType ifType;
    expect(LPAREN);
    ifType = parseExpression();
    if(ifType != BOOL){
        cout << "TYPE MISMATCH " << token.line_no << " C4\n";
        exit(1);
    }
    else{
        expect(RPAREN);
        parseBody();
    }
    return;
}

void Parser::parseWhile(){
    VariableType whileType;
    expect(LPAREN);
    whileType = parseExpression();
    if(whileType != BOOL){
        cout << "TYPE MISMATCH " << token.line_no << " C4\n";
        exit(1);
    }
    else{
        expect(RPAREN);
        parseBody();
    }
    return;
}

void Parser::parseSwitch(){
    VariableType switchType;
    expect(LPAREN);
    switchType = parseExpression();
    if(switchType != INT){
        cout << "TYPE MISMATCH " << token.line_no << " C5\n";
        exit(1);
    }
    else{
        expect(RPAREN);
        parseBody();
    }
    return;
}

void Parser::parseCaseList(){
    token = tokenList[index];
    if(token.lexeme != "CASE"){
        SyntaxError();
    }
    else{ //this is parseCase() tbh
        index++;
        expect(NUM);
        expect(COLON);
        parseBody();
    }
    token = tokenList[index];
    if(token.lexeme == "CASE"){
        parseCaseList();
    }
    return;
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

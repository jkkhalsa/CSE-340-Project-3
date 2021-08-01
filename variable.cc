#include <iostream>
#include "variable.h"

using namespace std;

string reservedTypes[] = {"UNKNOWN", "INT", "REAL", "BOOL"};


Variable::Variable(){
    type = UNKNOWN;
}


string Variable::printVariable(){
    string temp = "";
    if(type != UNKNOWN){
        temp = name + ": ";
        switch(type){
            case INT:
                temp += "int";
            case REAL:
                temp += "real";
            case BOOL:
                temp += "bool";
        }
        temp += "#\n";
        return temp;
    }
    
}


VariableList::VariableList(){

}

//name, type
void VariableList::addKnownVariable(string n, VariableType t){
    //create new variable
    Variable variable;
    variable.name = n;
    variable.type = t;
    variable.unknownNum = 0;

    //add it to the symbol table
    knownList.push_back(variable);
    return;
}

//name, type, number (if one is known){
void VariableList::addUnknownVariable(string n, int u){
    //create new variable
    Variable variable;
    variable.name = n;
    variable.type = UNKNOWN;
    if(u != 0){
        variable.unknownNum = u;
    }
    else{
        variable.unknownNum = nextUnknown;
        nextUnknown++;
    }
}



/*
//Search through our known list to see if a variable type is already known
//if not, we return UNKNOWN and can search the unknown list
Variable VariableList::searchKnownList(string currentScope, string variableName){
    for(int i = list.size()-1; i >= 0; i--){
        if(list[i].name == variableName){
            if(list[i].scope == currentScope || list[i].isPublic || list[i].scope == ":"){
                return list[i];
            }
        }
    }
    //if we're down here then there's no variable in the entire list that matches
    //so just make one ez
    Variable variable;
    variable.scope = "?";
    variable.name = variableName;
    return variable;
}

void VariableList::printVariableList(){
    for(auto elem : knownList){
        cout << elem.printVariable() << "\n";
    }
    return;
}*/
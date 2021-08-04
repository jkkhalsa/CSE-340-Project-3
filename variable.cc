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
    list.push_back(variable);
    return;
}

//name, type, number (if one is known)
//if it's not related to another unknown, just put 0
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
    list.push_back(variable);
    return;
}


//go through the symbol table when we find an unknown is equal to another unknown or to a type
void VariableList::resolveUnknownVariables(int beingResolved, int resolvingTo, VariableType newType){
    //beingResolved is the one being switched
    //resolvingTo replaces beingResolved
    //if we're resolving to a type, newType will not be Unknown
    if(newType == UNKNOWN){
        //two unknowns are actually the same unknown
        for(int i = list.size()-1; i>=0; i--){
            if(list[i].type == UNKNOWN && list[i].unknownNum == beingResolved){
                list[i].unknownNum = resolvingTo;
            }
        }
    }
    else{
        //turns out all these unknowns are actually a proper type
        for(int i = list.size()-1; i >= 0; i--){
            if(list[i].type == UNKNOWN && list[i].unknownNum == beingResolved){
                list[i].unknownNum = 0;
                list[i].type = newType;
            }
        }
    }
}


//search through our symbol table to see if we can find the variable in question
//if we can't just add a new unknown variable
Variable VariableList::searchList(string variableName){
    for(int i = list.size()-1; i >= 0; i--){
        if(list[i].name == variableName){
            checkedIndex = i;
            return list[i];
        }
    }
    //if we're down here then there's no variable in the entire list that matches
    //so this is new I guess ughhhhhh
    addUnknownVariable(variableName, 0);
    checkedIndex = list.size()-1;
    return list[checkedIndex];
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
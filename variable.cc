#include <iostream>
#include "variable.h"

using namespace std;

string reservedTypes[] = {"UNKNOWN", "INT", "REAL", "BOOL"};

string Variable::printVariable(){
    if(scope == ":"){
        return scope + ':' + name;
    }
    return scope + '.' + name;
}


VariableList::VariableList(){

}

//scope, name, isPublic?
void VariableList::addVariable(string s, string n, bool p){
    //create new variable
    Variable variable;
    variable.scope = s;
    variable.name = n;
    variable.isPublic = p;

    //add it to the symbol table
    list.push_back(variable);
    return;
}

//goes through the variable list and erases the ones belonging to the scope we just exited
void VariableList::eraseScope(string deletingScope){
    for(int i = 0; i < (int)list.size(); i++){
        if(list[i].scope == deletingScope){
            list.erase(list.begin() + i);
            i--;
        }
    }
    //printVariableList();
    return;
}

//Search through our symbol table to see where the one that just got called is at
//pass current scope, then variable name we're looking for
Variable VariableList::searchList(string currentScope, string variableName){
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
    for(auto elem : list){
        cout << elem.printVariable() << "\n";
    }
    return;
}
#include <iostream>
#include "variable.h"
#include <algorithm>

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
                break;
            case REAL:
                temp += "real";
                break;
            case BOOL:
                temp += "bool";
                break;
        }
        temp += " #";
    }
    else{
        temp = name;
    }
    return temp;
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
                //cout << "DEBUG: resolving variable " << list[i].name << " to type " << list[i].type << " and unknown " << list[i].unknownNum << "\n";
            }
        }
    }
    else{
        //turns out all these unknowns are actually a proper type
        for(int i = list.size()-1; i >= 0; i--){
            if(list[i].type == UNKNOWN && list[i].unknownNum == beingResolved){
                list[i].unknownNum = 0;
                list[i].type = newType;
                //cout << "DEBUG: resolving variable " << list[i].name << " to type " << list[i].type << " and unknown " << list[i].unknownNum << "\n";
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
            storeUnknown = list[checkedIndex].unknownNum;
            return list[i];
        }
    }
    //if we're down here then there's no variable in the entire list that matches
    //so this is new I guess ughhhhhh
    addUnknownVariable(variableName, 0);
    checkedIndex = list.size()-1;
    storeUnknown = list[checkedIndex].unknownNum;
    return list[checkedIndex];
}

void VariableList::printVariableList(){
    string unknownString;
    int unknownPrinting;
    bool skipFirst = false;
    vector <int> printedUnknowns;
    for(auto elem : list){
        if(elem.type != UNKNOWN){
            cout << elem.printVariable() << "\n";
        }
        else{
            unknownPrinting = elem.unknownNum;
            if(count(printedUnknowns.begin(), printedUnknowns.end(), unknownPrinting) == 0){
                //if the unknown we hit upon isn't in the list of what's already been printed
                unknownString = elem.printVariable();
                skipFirst = false;
                for(auto elem : list){
                    if(elem.unknownNum == unknownPrinting && skipFirst){
                        unknownString += ", ";
                        unknownString += elem.name;
                    }
                    //need this one to make sure the first one you hit in the list doesn't get printed twice
                    if(elem.unknownNum == unknownPrinting && !skipFirst){
                        skipFirst = true;
                    }
                }
                unknownString += ": ? #\n";
                cout << unknownString;
            }
            //add this unknown to the list of ones that've been printed
            printedUnknowns.push_back(unknownPrinting);
        }
    }
    return;
}

void VariableList :: debugPrintVariableList(){
    for(auto elem : list){
        cout << "<" << elem.name << ", type " << elem.type << ", unknown " << elem.unknownNum << ">\n";
    }
}
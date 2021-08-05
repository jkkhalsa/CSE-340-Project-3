#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <iostream>

//type types

typedef enum {UNKNOWN = 0,
    REAL,
    INT,
    BOOL
} VariableType;

class Variable{

    public:
    std::string name;
    VariableType type;
    int unknownNum;

    Variable();
    std::string printVariable();
};

class VariableList{

    public:
    std::vector <Variable> list;
    int nextUnknown;
    int checkedIndex; //for pointing to the variable returned by search
    int storeUnknown;  //for returning unknowns all the way up a chain of expressions


    VariableList();
    void addKnownVariable(std::string name, VariableType type); 
    void addUnknownVariable(std::string name, int declareUnknown);
    void resolveUnknownVariables(int beingResolved, int resolvingTo, VariableType newType);
    Variable searchList(std::string);
    
    void printVariableList();
    void debugPrintVariableList();

    private:
    std::vector<int> printedUnknowns;


};

#endif
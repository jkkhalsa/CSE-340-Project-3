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
    std::vector <Variable> knownList;
    std::vector <Variable> unknownList;
    int nextUnknown;


    VariableList();
    void addKnownVariable(std::string name, VariableType type); 
    void addUnknownVariable(std::string name, int declareUnknown);
    Variable searchList(std::string, std::string);
    
    void printVariableList();


};

#endif
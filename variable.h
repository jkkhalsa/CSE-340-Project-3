#ifndef VARIABLE_H
#define VARIABLE_H

#include <vector>
#include <iostream>

class Variable{

    public:
    std::string scope;
    std::string name;
    bool isPublic;

    std::string printVariable();
};

class VariableList{

    public:
    std::vector <Variable> list;

    VariableList();
    void addVariable(std::string, std::string, bool); 
    void eraseScope(std::string);
    Variable searchList(std::string, std::string);
    
    void printVariableList();


};

#endif
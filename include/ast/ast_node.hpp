#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>
#include <vector>
#include <string>

#include "compiler/frame.hpp"

class Node;

extern std::vector<std::string> g_variables;

typedef const Node* NodePtr;
typedef std::vector<NodePtr>* VectorPtr;
typedef const std::string* StrPtr;
    
class Node{
public:
    virtual ~Node(){}

    virtual void print(std::ostream& dst) const =0;
    virtual void printPy(std::ostream& dst, int depth = 0)const{};
    virtual int getLength()const{return -1;}
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const{};

};

class StringNode: public Node{
protected:
    const std::string* id;
    
public:
    StringNode(const std::string* _id) 
        :id(_id){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *id;
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        for(int i(0); i < depth; ++i) dst << "\t";
        dst << *id;
    }
    
    void addGlobal()const{
        std::string global = *id;
        g_variables.push_back(global);
    }
    
    std::string getId()const{
        return std::string(*id);
    }

};

class Expr: public Node{
public:
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const {}
    virtual void printMipsENegate(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const {}
};
class Statement: public Node{};

#endif

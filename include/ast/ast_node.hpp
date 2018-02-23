#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>
#include <vector>
#include <string>
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
    virtual void printMips(std::ostream& dst) const {};

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
    
    /*virtual void printMips(std::ostream& dst)const override {
        dst << *id;
    }*/
    
    std::string getId()const{
        return (*id);
    }
};

class Expr: public Node{};
class Statement: public Node{};

#endif

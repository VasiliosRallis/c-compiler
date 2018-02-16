#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>
#include <vector>
#include <string>
class Node;

typedef const Node* NodePtr;
typedef std::vector<NodePtr>* VectorPtr;
typedef const std::string* StrPtr;
    
class Node{
public:
    virtual ~Node(){}

    virtual void print(std::ostream& dst) const =0;
    virtual void printPy(std::ostream& dst) const=0;
    virtual int getLength()const{return -1;}

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
    
    virtual void printPy(std::ostream& dst)const override{
        dst << *id;
    }
};
#endif

#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>

class Node;

typedef const Node* NodePtr;

class Node{
public:
    virtual ~Node(){}

    virtual void print(std::ostream& dst) const =0;

};

#endif

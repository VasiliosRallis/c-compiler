#ifndef ast_expression_hpp
#define ast_expression_hpp

#include <iostream>

class Node;

typedef const Node* NodePtr;
    
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

class List: public Node{
protected:
    NodePtr l;
    NodePtr r;
    
public:
    List(NodePtr _l, NodePtr _r)
        :l(_l), r(_r){ }
   
   virtual void print(std::ostream& dst)const override{
	dst << "List" ;
        l->print(dst);
        dst << ",";
        r->print(dst);
   }
   virtual void printPy(std::ostream& dst) const override{}
};

#endif

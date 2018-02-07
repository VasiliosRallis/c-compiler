#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

class VarType: public Node{
private:
    std::string type;
    
public:
    VarType(const std::string& _type)
        : type(_type){}

    virtual void print(std::ostream& dst) const override{
        dst<<type;
    }
  
};

class VarDeclr: public Node{
private:
    NodePtr varType;    
    std::string declarator;

public:
    VarDeclr(NodePtr _varType, const std::string& _declarator)
        : varType(_varType), declarator(_declarator) {}

    virtual void print(std::ostream& dst) const override{
        varType->print(dst);
        dst << " ";
        dst << declarator;
        dst << ";";
    }
};

class DeclrList: public Node{
private:
	NodePtr declrList;    
	NodePtr varDeclr;
    
public:
    DeclrList(NodePtr _declrList, NodePtr _varDeclr)
        : declrList(_declrList), varDeclr(_varDeclr) {}

    virtual void print(std::ostream& dst) const override{
        declrList->print(dst);
	    varDeclr ->print(dst);
    }

};

class Block: public Node{
private:
	NodePtr declrList;    
    
public:
    Block(NodePtr _declrList)
        : declrList(_declrList) {}

    virtual void print(std::ostream& dst) const override{
	    dst << "{";
        declrList->print(dst);
	    dst << "}";
    }

};

class FunctionDef: public Node{
private:
	NodePtr varType;
	std::string id;    
	NodePtr block;
    
public:
    FunctionDef(NodePtr _varType, std::string _id, NodePtr _block)
        : varType(_varType), id(_id), block(_block) {}

    virtual void print(std::ostream& dst) const override{
	    varType->print(dst);
        dst << " " << id << "()";
	    block->print(dst);
    }

};

#endif

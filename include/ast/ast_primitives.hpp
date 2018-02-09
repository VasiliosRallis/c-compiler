#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

class VarType: public Node{
private:
    const std::string* type;
    
public:
    VarType(const std::string* _type)
        : type(_type){}

    virtual void print(std::ostream& dst) const override{
        dst<< *type;
    }
    
    virtual ~VarType() override{
            delete type;
     }
};

class VarDeclr: public Node{
private:
    NodePtr varType;    
    NodePtr identifierList;

public:
    VarDeclr(NodePtr _varType, NodePtr _identifierList)
        : varType(_varType), identifierList(_identifierList) {}

    virtual void print(std::ostream& dst) const override{
        varType->print(dst);
        dst << " ";
        identifierList->print(dst);
        dst << ";";
    }
    
    virtual ~VarDeclr() override{
        delete varType;
        delete identifierList;
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
    
    virtual ~DeclrList() override{
        delete declrList;
        delete varDeclr;
    }

};

class Block: public Node{
private:
	NodePtr declrList;    
    
public:
    Block(NodePtr _declrList = NULL)
        : declrList(_declrList) {}
        
    virtual void print(std::ostream& dst) const override{
	    dst << "{";
	    
	    if(declrList != NULL){
            declrList->print(dst);
        }
	    dst << "}";
    }
    
    virtual ~Block() override{
        delete declrList;
    }

};

class FunctionDef: public Node{
private:
	NodePtr varType;
	NodePtr identifier;   
	NodePtr block;
    
public:
    FunctionDef(NodePtr _varType, NodePtr _identifier, NodePtr _block)
        : varType(_varType), identifier(_identifier), block(_block) {}

    virtual void print(std::ostream& dst) const override{
	    varType->print(dst);
        dst << " ";
        identifier->print(dst);
        dst << "()";
	    block->print(dst);
    }
    
    virtual ~FunctionDef() override{
        delete varType;
        delete identifier;
        delete block;
    }
    
};


class Program: public Node{
private:
    NodePtr program;
    NodePtr basicProgram;

public:
    Program(NodePtr _program, NodePtr _basicProgram)
        :program(_program), basicProgram(_basicProgram){}
        
    virtual void print(std::ostream& dst) const override{
	    program->print(dst);
	    dst << " ";
	    basicProgram->print(dst);
    }
    
    virtual ~Program() override{
        delete program;
        delete basicProgram;
    }
};

class Identifier: public Node{
private:
    const std::string* id;
    
public:
    Identifier(const std::string* _id)
        :id(_id){}
        
    virtual void print(std::ostream& dst) const override{
        dst << *id;
    }

    virtual ~Identifier() override{
        delete id;
    }
};


class IdentifierList: public Node{
private:
    NodePtr identifierList;
    NodePtr identifier;
    
public:
    IdentifierList(NodePtr _identifierList, NodePtr _identifier)
        :identifierList(_identifierList), identifier(_identifier){}
    
    virtual void print(std::ostream& dst) const override{
        identifierList->print(dst);
        dst << ", ";
        identifier->print(dst);
    }
    
    virtual ~IdentifierList() override{
        delete identifierList;
        delete identifier;
    }
};
#endif

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

class StatementList: public Node{
private:
	NodePtr statementList;    
	NodePtr statement;
    
public:
    StatementList(NodePtr _statementList, NodePtr _statement)
        : statementList(_statementList), statement(_statement) {}

    virtual void print(std::ostream& dst) const override{
        statementList->print(dst);
	statement ->print(dst);
    }
    
    virtual ~StatementList() override{
        delete statementList;
        delete statement;
    }

};

class VarInit : public Node {
private :
	NodePtr varType;
	NodePtr idlist;
	NodePtr initializer;

public:
	VarInit(NodePtr _varType, NodePtr _idlist, NodePtr _initializer)
	: varType(_varType) , idlist(_idlist) , initializer(_initializer) {}
	
	virtual void print(std::ostream& dst) const override{
		varType->print(dst);
		dst << " ";
		idlist ->print(dst);
		dst << " = ";
		initializer -> print(dst);
		dst << ";";
    }
    
    virtual ~VarInit() override{
        delete varType;
        delete idlist;
	delete initializer;
    }
};

class Block: public Node{
private:
	NodePtr declrList;
	NodePtr statementList;    
    
public:
    Block(NodePtr _declrList = NULL , NodePtr _statementList = NULL)
        : declrList(_declrList), statementList(_statementList) {}
        
    virtual void print(std::ostream& dst) const override{
	    dst << "{";
	    
	    if(declrList != NULL){
            declrList->print(dst);
      	}
      	
	    if(statementList != NULL){
            statementList->print(dst);
      	}

	    dst << "}";
    }
    
    virtual ~Block() override{
        delete declrList;
        delete statementList;
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

class IntConst: public Node{
private:
    const std::string* value;
    
public:
    IntConst(const std::string* _value)
        :value(_value){}
        
    virtual void print(std::ostream& dst) const override{
        dst << std::stoi((*value));
    }

    virtual ~IntConst() override{
        delete value;
    }
};

class TypeQualifier: public Node{
private:
    const std::string* id;
    
public:
    TypeQualifier(const std::string* _id)
        :id(_id){}
        
    virtual void print(std::ostream& dst) const override{
        dst << *id;
    }
    
    virtual ~TypeQualifier() override{
        delete id;
    }  
};

class DeclSpecifierList: public Node{
private:
    NodePtr declSpecifierList;
    NodePtr declSpecifier;
    
public:
    DeclSpecifierList(NodePtr _declSpecifierList, NodePtr _declSpecifier)
        :declSpecifierList(_declSpecifierList), declSpecifier(_declSpecifier){}
        
    virtual void print(std::ostream& dst) const override{
        declSpecifierList->print(dst);
        dst << " ";
        declSpecifier->print(dst);
    }
    
    ~DeclSpecifierList(){
        delete declSpecifierList;
        delete declSpecifier;
     }
};

class StorClassSpec: public Node{
private:
    const std::string* id;
    
public:
    StorClassSpec(const std::string* _id)
        :id(_id){}
        
        
    virtual void print(std::ostream& dst) const override{
        dst << *id;
    }
    
    virtual ~StorClassSpec() override{
        delete id;
    }
};

class StrLit: public Node{
private:
    const std::string* id;
    
public:
    StrLit(const std::string* _id)
        :id(_id){}
        
        
    virtual void print(std::ostream& dst) const override{
        dst << *id;
    }
    
    virtual ~StrLit () override{
        delete id;
    }
};

class Expr: public Node{
private:
    NodePtr primaryExpr1;
    NodePtr primaryExpr2;
    
public:
    Expr(NodePtr _primaryExpr1, NodePtr _primaryExpr2)
        :primaryExpr1(_primaryExpr1), primaryExpr2(_primaryExpr2){}
        
    virtual void print(std::ostream& dst) const override{
        primaryExpr1->print(dst);
        dst<< "=";
        primaryExpr2->print(dst);
    }
    
    virtual ~Expr() override{
        delete primaryExpr1;
        delete primaryExpr2;
   }
};

class IfStatement: public Node{
private:
    NodePtr expr;
    NodePtr statement1;
    NodePtr statement2;
    
public:
    IfStatement(NodePtr _expr, NodePtr _statement1, NodePtr _statement2 = NULL)
        :expr(_expr), statement1(_statement1), statement2(_statement2){}
        
    virtual void print(std::ostream& dst) const override{
        dst << "if(";
        expr->print(dst);
        dst << ")";
        statement1->print(dst);
        if(statement2 != NULL){
            dst << "else ";
            statement2->print(dst);
        }
    }
    
    virtual ~IfStatement() override{
        delete expr;
        delete statement1;
        delete statement2;
    }
};

class CaseStatement: public Node{
private:
    NodePtr expr;
    NodePtr statement;
    
public:
    CaseStatement(NodePtr _expr, NodePtr _statement)
        :expr(_expr), statement(_statement){}
        
    virtual void print(std::ostream& dst) const override{
        dst << "switch(";
        expr->print(dst);
        dst << ")";
        statement->print(dst);
    }
    
    virtual ~CaseStatement() override{
        delete expr;
        delete statement;
    }
};

class WhileStatement: public Node{
private:
    NodePtr expr;
    NodePtr statement;
    
public:
    WhileStatement(NodePtr _expr, NodePtr _statement)
        :expr(_expr), statement(_statement){}
        
        
    virtual void print(std::ostream& dst) const override{
        dst << "while(";
        expr->print(dst);
        dst << ")";
        statement->print(dst);
    }
    
    virtual ~WhileStatement() override{
        delete expr;
        delete statement;
    }
};
#endif

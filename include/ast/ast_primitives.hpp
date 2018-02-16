#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>
#include <vector>

extern int g_depth;
//extern std::string g_variables;

//class Identifier: public Node;

class Declaration : public Node {
private:
    NodePtr declrspecList;    
    std::vector<NodePtr>* initdeclrList;
public:
    Declaration(NodePtr _declrspecList, std::vector<NodePtr>* _initdeclrList)
        : declrspecList(_declrspecList), initdeclrList(_initdeclrList) {}
    
    virtual void print(std::ostream& dst) const override{
        declrspecList->print(dst);
	    dst << " ";
		for(int i(0); i < initdeclrList->size(); ++i){       
        	initdeclrList->at(i)->print(dst);
			if(i < initdeclrList->size() - 1) dst << ",";
	    }
        dst << ";";
    }
    
    virtual void printPy(std::ostream& dst) const override{ }
};

class InitDeclarator : public Node {
private:
    NodePtr directDeclarator;    
    NodePtr asgnExpr;
public:
    InitDeclarator(NodePtr _directDeclarator, NodePtr _asgnExpr)
        : directDeclarator(_directDeclarator), asgnExpr(_asgnExpr) {}
    
    virtual void print(std::ostream& dst) const override{
        directDeclarator->print(dst);
	dst << "=";      
        asgnExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst) const override{ }
       
    virtual ~InitDeclarator() override{
        delete directDeclarator;
        delete asgnExpr;
    }

};

class FunctionDeclaration : public Node {
private:
    NodePtr directDeclarator;    
public:
    FunctionDeclaration(NodePtr _directDeclarator)
        : directDeclarator(_directDeclarator) {}
    
    virtual void print(std::ostream& dst) const override{
        directDeclarator->print(dst);
	dst << "()";      
    }
    
    virtual void printPy(std::ostream& dst) const override{ }
       
    virtual ~FunctionDeclaration() override{
        delete directDeclarator;
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
    
    virtual void printPy(std::ostream& dst) const override{
        declrList->printPy(dst);
        dst << "\n";
        varDeclr->printPy(dst);
    }
    
    virtual ~DeclrList() override{
        delete declrList;
        delete varDeclr;
    }

};

class StatementList: public List{
public:
    StatementList(NodePtr _l, NodePtr _r)
        :List(l, r){};

    virtual void print(std::ostream& dst) const override{
        l->print(dst);
	    r->print(dst);
    }
    
    virtual void printPy(std::ostream& dst) const override{
        l->printPy(dst);
        dst << "\n";
        r->printPy(dst);
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
    
    virtual void printPy(std::ostream& dst) const override{
        for(int i(0); i < g_depth; ++i){
            dst << "\t";
        }
        idlist->printPy(dst);
        dst << "=";
        initializer->printPy(dst);
        int l = idlist->getLength();
        for(int i(0); i < l-1; ++i){
            dst << ",";
            initializer->printPy(dst);
        }
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
    

    virtual void printPy(std::ostream& dst) const override{
        g_depth++;
        if(declrList  != NULL){
            declrList->printPy(dst);
            dst << "\n";
        }
        if(statementList != NULL){
            statementList->printPy(dst);
            dst << "\n";
        }
        g_depth--;   
    }
    
    virtual ~Block() override{
        delete declrList;
        delete statementList;
    }

};

class FunctionDef: public Node{
private:
	NodePtr varType;
	NodePtr directDeclarator;   
	NodePtr block;
    
public:
    FunctionDef(NodePtr _varType, NodePtr _directDeclarator, NodePtr _block)
        : varType(_varType), directDeclarator(_directDeclarator), block(_block) {}
        

    virtual void print(std::ostream& dst) const override{
	    varType->print(dst);
        dst << " ";
        directDeclarator->print(dst);
	    block->print(dst);
    }
    
    virtual void printPy(std::ostream& dst) const override{    
	 dst << "def ";
        directDeclarator->printPy(dst);
        dst << "():\n";
       /* for(int i(0); i < g_depth + 1; ++i){
           dst << "\t";
        }*/
        //if(g_variables != ""){
        //    dst << "global ";
        //    dst << g_variables;
        //}
        
        block->printPy(dst);
    }
    
    virtual ~FunctionDef() override{
        delete varType;
        delete directDeclarator;
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
    
    virtual void printPy(std::ostream& dst) const override{
	program->printPy(dst);
	dst << "\n";
	basicProgram->printPy(dst);
    //    if(dynamic_cast<const VarDeclr*>(program)){
    //       dynamic_cast<const VarDeclr*>(program)->getGlobal(); 
    //    }
    }
    
    virtual ~Program() override{
        delete program;
        delete basicProgram;
    }
};

class IntConst: public StringNode{
public:
    IntConst(const std::string* _id)
        :StringNode(_id){}
        
    virtual void print(std::ostream& dst) const override{
        dst << std::stoi((*id));
    }
    virtual void printPy(std::ostream& dst) const override{
        dst << std::stoi((*id));
    }
};

class DeclSpecifierList: public List{
public:
    DeclSpecifierList(NodePtr _l, NodePtr _r)
        :List(_l, _r){}
        
    virtual void print(std::ostream& dst) const override{
        l->print(dst);
        dst << " ";
        r->print(dst);
    }
        
    virtual void printPy(std::ostream& dst) const override{}
};

class ExprStatement: public Node{
private:
    NodePtr expr;    
public:
    ExprStatement(NodePtr _expr)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst) const override{
        if(expr != NULL){
            expr->print(dst);
            dst << ";";
        }else{
            dst << ";";
        }
    }
    
    virtual void printPy(std::ostream& dst) const override{
	for(int i(0); i < g_depth; ++i){
		dst << "\t";
	}
	expr->printPy(dst);
    }
    
    virtual ~ExprStatement() override{
        delete expr;
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
    
    virtual void printPy(std::ostream& dst) const override{
        for(int i(0); i < g_depth; ++i){
            dst << "\t";
        }
        dst << "if(";
        expr->printPy(dst);
        dst << ")";
        dst << "\n";
        statement1->printPy(dst);
        dst << "\n";
        if(statement2 != NULL){
            for(int i(0); i < g_depth; ++i){
                dst << "\t";
            }
            dst << "else:";
            dst << "\n";
            statement2->printPy(dst);
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
    
    virtual void printPy(std::ostream& dst) const override{}
    
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
    
    virtual void printPy(std::ostream& dst) const override{}
    
    virtual ~WhileStatement() override{
        delete expr;
        delete statement;
    }
};

class DoStatement: public Node{
private:
    NodePtr statement;
    NodePtr expr;
    
public:
    DoStatement(NodePtr _statement, NodePtr _expr)
        :statement(_statement), expr(_expr){}
        
        
    virtual void print(std::ostream& dst) const override{
        dst << "do ";
        statement->print(dst);
        dst << "while(";
        expr->print(dst);
        dst << ");";
    }
    
    virtual void printPy(std::ostream& dst) const override{}
    
    virtual ~DoStatement() override{
        delete statement;
        delete expr;
    }
};

class ForStatement: public Node{
private:
    NodePtr expr1;
    NodePtr expr2;
    NodePtr expr3;
    NodePtr statement;
    
public:
    ForStatement(NodePtr _expr1, NodePtr _expr2, NodePtr _expr3, NodePtr _statement)
        :expr1(_expr1), expr2(_expr2), expr3(_expr3), statement(_statement){}
        
    virtual void print(std::ostream& dst) const override{
        dst << "for(";
        if(expr1 != NULL){
            expr1->print(dst);
        }
        dst << ";";
        if(expr2 != NULL){
            expr2->print(dst);
        }
        dst << ";";
        if(expr3 != NULL){
            expr3->print(dst);
        }
        dst << ")";
        statement->print(dst);
    }
    
    virtual void printPy(std::ostream& dst) const override{}
    
    virtual ~ForStatement() override{
        delete expr1;
        delete expr2;
        delete expr3;
        delete statement;
    }
};

class PrimaryExpr: public Node{
private:
    NodePtr expr;

public:
    PrimaryExpr(NodePtr _expr)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst)const override{
        dst << "(";
        expr->print(dst);
        dst << ")";
    }
    
    virtual void printPy(std::ostream& dst) const override{
	expr -> printPy(dst);	
	} 
};

class Operator: public StringNode{
public:
    enum Type{
        MUL,
        ADD,
        SHIFT,
        RELATIONAL,
        EQUALITY,
        AND,
        EXCLUSIVE_OR,
        INCLUSIVE_OR,
        LOGICAL_AND,
        LOGICAL_OR,
        ASSIGN
    };  
    Operator(const std::string* _id, Operator::Type _type)
        :StringNode(_id), type(_type){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *id;
    }
    
    virtual void printPy(std::ostream& dst)const override{
        dst << *id;
    }

protected:
   Type type;
};


class UnaryExpr: public Node{
private:
    const std::string* oper;
    NodePtr postfixExpr;
public:
    UnaryExpr(const std::string* _oper, NodePtr _postfixExpr)
        :oper(_oper), postfixExpr(_postfixExpr){}
        
    virtual void print(std::ostream& dst) const override{
        dst << *oper;
        postfixExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst)const override{}
};

class ConditionalExpr: public Node{
private:
    NodePtr logicalOrExpr;
    NodePtr expr;
    NodePtr conditionalExpr;

public:
    ConditionalExpr(NodePtr _logicalOrExpr, NodePtr _expr, NodePtr _conditionalExpr)
        :logicalOrExpr(_logicalOrExpr), expr(_expr), conditionalExpr(_conditionalExpr){}
        
    virtual void print(std::ostream& dst)const override{
        logicalOrExpr->print(dst);
        dst << " ? ";
        expr->print(dst);
        dst << " : ";
        conditionalExpr->print(dst);
    }
    virtual void printPy(std::ostream& dst)const override{}
};


class BinaryOperation: public Node{
private:
    NodePtr operand1;
    NodePtr oper;
    NodePtr operand2;
    
public:
    BinaryOperation(NodePtr _operand1, NodePtr _oper, NodePtr _operand2)
        :operand1(_operand1), oper(_oper), operand2(_operand2){}
        
    virtual void print(std::ostream& dst)const override{
        operand1->print(dst);
        oper->print(dst);
        operand2->print(dst);
    }
    
    virtual void printPy(std::ostream& dst)const override{}
};

class CastExpr: public Node{
private:
    NodePtr typeName;
    NodePtr unaryExpr;
    
public:
    CastExpr(NodePtr _typeName, NodePtr _unaryExpr)
        :typeName(_typeName), unaryExpr(_unaryExpr){}
        
    virtual void print(std::ostream& dst)const override{
        dst << "(";
        typeName->print(dst);
        dst << ")";
        unaryExpr->print(dst);
    }
   
   virtual void printPy(std::ostream& dst)const override{}
   
   virtual ~CastExpr()override{
        delete typeName;
        delete unaryExpr;
   }
};

class PostfixExpr: public Node{
private:
    NodePtr primaryExpr;
    const std::string* oper1;
    NodePtr operand;
    const std::string* oper2;
    
public:
    PostfixExpr(NodePtr _primaryExpr, const std::string* _oper1, NodePtr _operand, const std::string* _oper2)
        :primaryExpr(_primaryExpr), oper1(_oper1), operand(_operand), oper2(_oper2){}
        
    virtual void print(std::ostream& dst)const override{
        primaryExpr->print(dst);
        if(oper1 != NULL){
            dst << *oper1;
        }
        if(operand != NULL){
            operand->print(dst);
        }
        if(oper2 != NULL){
            dst << *oper2;
        }
    }
    
    virtual void printPy(std::ostream& dst)const override{}
    
    virtual ~PostfixExpr()override{
        delete primaryExpr;
        delete oper1;
        delete operand;
        delete oper2;
    }
};
  
class DeclSpecifier: public StringNode{
public:
    DeclSpecifier(const std::string* _id)
        :StringNode(_id){}
};

class LabeledStatement: public StringNode{
private:
    NodePtr p1;
    NodePtr p2;
    
public:
    LabeledStatement(const std::string* _id, NodePtr _p1, NodePtr _p2)
        :StringNode(_id), p1(_p1), p2(_p2){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *id << " ";
        if(p1 != NULL)
            p1->print(dst);
        dst << ":";
        p2->print(dst);
    }
    
    virtual void printPy(std::ostream& dst)const override{}
};

class JumpStatement: public StringNode{
private:
    const std::string* str1;
    NodePtr p1;
    
public:
    JumpStatement(const std::string * _id, const std::string* _str1, NodePtr _p1)
        :StringNode(_id), str1(_str1), p1(_p1){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *id << " ";
        if(str1 != NULL)
            dst << *str1;
        if(p1 != NULL)
            p1->print(dst);
        dst << ";";
    }
    
    virtual void printPy(std::ostream& dst)const override{}
};
#endif

#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <iostream>

extern int g_depth;
//extern std::string g_variables;

//class Identifier: public Node;

class VarType: public Node{
private:
    const std::string* type;
    
public:
    VarType(const std::string* _type)
        : type(_type){}

    virtual void print(std::ostream& dst) const override{
        dst<< *type;
    }
    
    virtual void printPy(std::ostream& dst) const override{}
    
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
        
    
    //void getGlobal(){
    //    (dynamic_cast<const Identifier*>(identifierList))->getGlobal();
    //}

    virtual void print(std::ostream& dst) const override{
        varType->print(dst);
        dst << " ";
        identifierList->print(dst);
        dst << ";";
    }
    
    virtual void printPy(std::ostream& dst) const override{
        for(int i(0); i < g_depth; ++i){
            dst << "\t";
        }
        identifierList->printPy(dst); 
        dst << "=0";
        int l = identifierList->getLength();
        for(int i(0); i < l-1; ++i){
            dst << ",0";
        }
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
    
    virtual void printPy(std::ostream& dst) const override{
        statementList->printPy(dst);
        dst << "\n";
        statement->printPy(dst);
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
    
    virtual void printPy(std::ostream& dst) const override{
        dst << "def ";
        identifier->printPy(dst);
        dst << "():\n";
        for(int i(0); i < g_depth + 1; ++i){
           dst << "\t";
        }
        //if(g_variables != ""){
        //    dst << "global ";
        //    dst << g_variables;
        //}
        
        block->printPy(dst);
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
    
    virtual void printPy(std::ostream& dst) const override{
    //    if(dynamic_cast<const VarDeclr*>(program)){
    //       dynamic_cast<const VarDeclr*>(program)->getGlobal(); 
    //    }
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
        
    //void getGlobal(){
    //    g_variables = *id;
    //}
        
    virtual void print(std::ostream& dst) const override{
        dst << *id;
    }
    
    virtual void printPy(std::ostream& dst) const override{
        dst << *id;
    }
    
    int getLength() const override{return 1;}

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
    
    int getLength()const override{
        return identifierList->getLength() + identifier->getLength();
    }
    
    virtual void printPy(std::ostream& dst) const override{
        identifierList->printPy(dst);
        dst<<",";
        identifier->printPy(dst);
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
    
    virtual void printPy(std::ostream& dst) const override{
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
    
    virtual void printPy(std::ostream& dst) const override{}
    
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
    
    virtual void printPy(std::ostream& dst) const override{}
    
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
    
    virtual void printPy(std::ostream& dst) const override{}
    
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
    
    virtual void printPy(std::ostream& dst) const override{}
    
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
    
    virtual void printPy(std::ostream& dst) const override{
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
    
    virtual void printPy(std::ostream& dst) const override{}
        
    ~PrimaryExpr() override{
        delete expr;
     }   
};

class Operator: public Node{
private:
    const std::string* oper;
public:
    Operator(const std::string* _oper)
        :oper(_oper){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *oper;
    }
    
    virtual void printPy(std::ostream& dst)const override{
        dst << *oper;
    }
    
    ~Operator(){
        delete oper;
     }
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

class AssignmentExpr: public Node{
private:
    NodePtr unaryExpr;
    NodePtr assignmentOper;
    NodePtr assignmentExpr;
public:
    AssignmentExpr(NodePtr _unaryExpr, NodePtr _assignmentOper, NodePtr _assignmentExpr)
        :unaryExpr(_unaryExpr), assignmentOper(_assignmentOper), assignmentExpr(_assignmentExpr){}
        
    virtual void print(std::ostream& dst)const override{
        unaryExpr->print(dst);
        assignmentOper->print(dst);
        assignmentExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst) const override{}
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


class ExprStatement: public Node{
private:
    NodePtr expr;

public:
    ExprStatement(NodePtr _expr = NULL)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst)const override{
        if(expr != NULL){
            expr->print(dst);
            dst << ";";
        }else{
            dst << ";";
        }
    }
  
    virtual void printPy(std::ostream& dst)const override{}
};

class BinaryOperation: public Node{
private:
    NodePtr operand1;
    std::string* oper;
    NodePtr operand2;
    
public:
    BinaryOperation(NodePtr _operand1, std::string* _oper, NodePtr _operand2)
        :operand1(_operand1), oper(_oper), operand2(_operand2){}
        
    virtual void print(std::ostream& dst)const override{
        operand1->print(dst);
        dst << " " << *oper << " ";
        operand2->print(dst);
    }
    
    virtual void printPy(std::ostream& dst)const override{}
    
    virtual ~BinaryOperation()override{
        delete operand1;
        delete oper;
        delete operand2;
     }
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

class ArgumentExprList: public Node{
private:
    NodePtr argumentExprList;
    NodePtr assignmentExpr;

public:
    ArgumentExprList(NodePtr _argumentExprList, NodePtr _assignmentExpr)
        :argumentExprList(_argumentExprList), assignmentExpr(_assignmentExpr){}
        
    virtual void print(std::ostream& dst)const override{
        argumentExprList->print(dst);
        dst << " ";
        assignmentExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst)const override{}
    
    virtual ~ArgumentExprList()override{
        delete argumentExprList;
        delete assignmentExpr;
    }
};   
#endif

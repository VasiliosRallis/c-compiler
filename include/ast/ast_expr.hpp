#ifndef ast_expr_hpp
#define ast_expr_hpp

class PrimaryExpr: public Expr{
private:
    NodePtr expr; //This can be everything a stringConst an intConst (might lead to future issues)

public:
    PrimaryExpr(NodePtr _expr)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst)const override{
        dst << "(";
        expr->print(dst);
        dst << ")";
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
	    expr -> printPy(dst);	
	} 
};

class UnaryExpr: public Expr{
private:
    StrPtr oper;
    NodePtr postfixExpr;
public:
    UnaryExpr(StrPtr _oper, NodePtr _postfixExpr)
        :oper(_oper), postfixExpr(_postfixExpr){}
        
    virtual void print(std::ostream& dst) const override{
        dst << *oper;
        postfixExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};

class ConditionalExpr: public Expr{
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
    virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};

class BinaryOperation: public Expr{
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
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        for(int i(0); i < depth; ++i) dst << "\t";
        operand1->printPy(dst);
        oper->printPy(dst);
        operand2->printPy(dst);
    }
};

class CastExpr: public Expr{
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
   
   virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};

class PostfixExpr: public Expr{
private:
    NodePtr primaryExpr;
    StrPtr oper1;
    VectorPtr argumentExprList;
    StrPtr oper2;
    
public:
    PostfixExpr(NodePtr _primaryExpr, StrPtr _oper1, VectorPtr _argumentExprList, StrPtr _oper2)
        :primaryExpr(_primaryExpr), oper1(_oper1), argumentExprList(_argumentExprList), oper2(_oper2){}
        
    virtual void print(std::ostream& dst)const override{
        primaryExpr->print(dst);
        if(oper1 != NULL){
            dst << *oper1;
        }
        if(argumentExprList != NULL){
            for(int i(0); i < argumentExprList->size(); ++i){
                argumentExprList->at(i)->print(dst);
                if(i < argumentExprList->size() - 1) dst << ",";
            }
        }
        if(oper2 != NULL){
            dst << *oper2;
        }
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};
  
class DeclSpecifier: public StringNode{
public:
    DeclSpecifier(StrPtr _id)
        :StringNode(_id){}
};

#endif

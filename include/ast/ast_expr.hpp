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
	
	virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const override{
	    if(dynamic_cast<const IntConst*>(expr)){
	        int id = dynamic_cast<const IntConst*>(expr)->getId();
	        dst << "li $t0, " << id << "\n";
	        framePtr->store(dst, "$t0", destName);
	    }
	    else if(dynamic_cast<const StringNode*>(expr)){
	        std::string id = dynamic_cast<const StringNode*>(expr)->getId();
	        framePtr->load(dst, "$t0", id);
	        framePtr->store(dst, "$t0", destName);
        }
        else if (dynamic_cast<const Expr*>(expr)){
            dynamic_cast<const Expr*>(expr)->printMipsE(dst,destName,framePtr);        
        }
    }
    
    std::string getId()const{
        if(dynamic_cast<const StringNode*>(expr)){
            std::string id = dynamic_cast<const StringNode*>(expr) ->getId() ;
            return id;
        }   
        else{
            std::cerr << "Tried to call getId on PrimaryExpr that wasnt stringNode" << std::endl;
            return "error" ;   
        } 
    
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
    const Expr* operand1;
    const Operator* oper;
    const Expr* operand2;
    
public:
    BinaryOperation(const Expr* _operand1, const Operator* _oper, const Expr* _operand2)
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
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr = NULL)const override{
        std::string n1 = makeName();
        std::string n2 = makeName();
        operand1->printMipsE(dst,n1,framePtr);
        std::cerr << "Was here" << std::endl;
        operand2->printMipsE(dst,n2,framePtr);
        
        framePtr->load(dst,"$t0",n1);
        framePtr->load(dst,"$t1",n2);        
        
        if(oper->getId() == "+"){
            dst << "add $t2, $t0, $t1" << std::endl ;
        }
        else if(oper->getId() == "-"){
            dst << "sub $t2, $t0, $t1" << std:: endl;
        }
        else if(oper->getId() == "*"){
            dst<< "mul $t2, $t0, $t1" << std :: endl;
        }
        else if (oper->getId() == "/"){
            dst<<"div $t0, $t1" << std:: endl;
            dst<<"mflo $t2" << std::endl;
        }
        else if (oper->getId() == "%"){
            dst<<"div $t0, $t1" << std:: endl;
            dst<<"mfhi $t2" << std::endl;
        }
        else if (oper->getId() == "==") {
            dst << "xor $t2, $t0, $t1" << std::endl;
            dst << "sltu $t2, $t2, 1" << std::endl;    
        }
        else if (oper->getId() == "!=") {
            dst << "xor $t2, $t0, $t1" << std::endl;
            dst << "sltu $t2, $0, $t2" << std::endl;    
        }
        else if(oper->getId() == "="){
            if(dynamic_cast<const PrimaryExpr*>(operand1)){
                std::string id = dynamic_cast<const PrimaryExpr*>(operand1)->getId();
                framePtr->store(dst, "$t1", id);
                dst << "move $t2, $t1\n";
            }else{
                std::cerr << "Left operand of assignment expr was not a PrimaryExpr\n";
           }
        }

        framePtr->store(dst,"$t2",destName);
        
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

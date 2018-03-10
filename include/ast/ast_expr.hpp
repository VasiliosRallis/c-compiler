#ifndef ast_expr_hpp
#define ast_expr_hpp    
#include "ast_real/ast/postfixExpr.hpp"
#include "ast_real/compiler/frame.hpp"
#include <cassert>

class PrimaryExpr: public Expr{
private:
    NodePtr expr; //This can be everything a stringConst an intConst (might lead to future issues)

public:
    PrimaryExpr(NodePtr _expr)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst)const override{
        if(dynamic_cast<const Expr*>(expr)){
            dst << "(";
            expr->print(dst);
            dst << ")";
        }else{
            expr->print(dst);
        }   
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        if(dynamic_cast<const Expr*>(expr)){
            dst << "(";
            expr->printPy(dst);
            dst << ")";
        }else{
            expr->printPy(dst);
        }   
    }
	
	virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override{
        if(dynamic_cast<const StringNode*>(expr)){
            std::string id = expr->getId();
	        Type myType = expr->getType(framePtr);
	        
	        if(type == Type::INT){
                if(expr->isIdentifier()){
	                if(myType == Type::INT || myType == Type::CHAR){
	                    framePtr->load(dst, "$t0", id);
	                    framePtr->store(dst, "$t0", destName, type);
	                
	                }else if(myType == Type::FLOAT){
	                    framePtr->load(dst, "$f0", id);
	                    TypeConv::convert(dst, type, Type::FLOAT, "$t0", "$f0");
	                    framePtr->store(dst, "$t0", destName, type);
    	                
	                }else{assert(0);}
	                
	            }else if(myType == Type::INT){
	                dst << "li $t0, " << id << "\n";
	                framePtr->store(dst, "$t0", destName, Type::INT);
	            
	            }else{assert(0);}

	        }else if(type == Type::CHAR){
	            if(expr->isIdentifier()){
	                if(myType == Type::CHAR){
	                    framePtr->load(dst, "$t0", id);
	                    framePtr->store(dst, "$t0", destName, Type::CHAR);
	                    
	                }else{assert(0);}
	                
	                
	            }else if(myType == Type::CHAR){
	                int ascii = (int)id[1];
	                dst << "li $t0, " << ascii << std::endl;
	                framePtr->store(dst, "$t0", destName, Type::CHAR);
	                
	            }else{assert(0);}
	            
	        }else{assert(0);}
	        
        }else if(dynamic_cast<const Expr*>(expr)){
            expr->printMipsE(dst, destName, framePtr, type);
                    
        }else{assert(0);}
        
    }
      
    virtual std::string getId()const override{
        return expr->getId();
    }
    
    
    virtual Type getType(const Frame* framePtr)const override{
        return expr->getType(framePtr);
    }
    
};

class UnaryExpr: public Expr{
private:
    StrPtr oper;
    const Expr* postfixExpr;
public:
    UnaryExpr(StrPtr _oper, const Expr* _postfixExpr)
        :oper(_oper), postfixExpr(_postfixExpr){}
        
    virtual void print(std::ostream& dst) const override{
        dst << *oper;
        postfixExpr->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        dst << *oper;
        postfixExpr->printPy(dst);        
    }
    
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override{
        if(*oper == "-"){            
            postfixExpr->printMipsE(dst, destName, framePtr, type);
	        framePtr->load(dst, "$t0", destName);
            dst << "sub $t0, $0, $t0" << std::endl;     //Negate variable value stored in $t0
	        framePtr->store(dst, "$t0", destName, type);
        }
        else if(*oper == "+"){            
            postfixExpr->printMipsE(dst, destName, framePtr, type);
        }
        else if (*oper == "!"){
            postfixExpr->printMipsE(dst, destName, framePtr, type);
            framePtr->load(dst, "$t0", destName);                    	
            dst << "sltu $t0, $t0, 1" << std::endl;
	        dst << "andi $t0, $t0, 0x00ff" << std::endl;
            framePtr-> store(dst, "$t0" , destName, type);
        }
        else if(*oper == "~"){
            postfixExpr->printMipsE(dst,destName,framePtr, type);
            framePtr->load(dst, "$t0", destName);                    	
            dst << "nor  $t0, $t0, $0" << std::endl;
            framePtr-> store(dst, "$t0", destName, type);           
        }
        else if(*oper == "++"){            
            postfixExpr->printMipsE(dst, destName, framePtr, type);
	        framePtr->load(dst, "$t0", destName);
            dst << "addi $t0, $t0, 1" << std::endl;     //Increment value before storing it back 
	        framePtr->store(dst, "$t0", destName, type);
            if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                std::string id = postfixExpr->getId();
                framePtr->store(dst, "$t0", id, type);
            }
        }
        else if(*oper == "--"){            
            postfixExpr->printMipsE(dst, destName, framePtr, type);
	        framePtr->load(dst, "$t0", destName);
            dst << "addi $t0, $t0, -1" << std::endl;     //Increment value before storing it back
	        framePtr->store(dst, "$t0", destName, type);
            if(dynamic_cast<const PrimaryExpr*>(postfixExpr)){
                std::string id = postfixExpr->getId();
                framePtr->store(dst, "$t0", id, type);
            }
        }
        else if(*oper == "&"){
            std::string id(postfixExpr->getId());
            framePtr->loadAddr(dst, "$t0", id);
            framePtr->store(dst, "$t0", destName, type);
        }
        else if(*oper == "*"){
            postfixExpr->printMipsE(dst, destName, framePtr, type);
            framePtr->load(dst, "$t0", destName);
            dst << "lw $t0, 0($t0)" << std::endl;
            framePtr->store(dst, "$t0", destName, type);
        }
    }
    
    virtual bool isAddr()const override{
        if(*oper == "&") return true;
        else return false;
    }
    
    virtual std::string getId()const override{
        return postfixExpr->getId();
    }
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
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override{
        std::string n1 = makeName("binary");
        std::string n2 = makeName("binary");
        
        //We don't need the first operand if we are doing an assignment
        if(oper->getId() != "="){ 
            operand1->printMipsE(dst, n1, framePtr, type);
            operand2->printMipsE(dst, n2, framePtr, type);
            framePtr->load(dst, "$t0", n1);
            framePtr->load(dst, "$t1", n2);
        
        }else{
            operand2->printMipsE(dst, n2, framePtr, type);
            framePtr->load(dst, "$t1", n2);
        }
        
        //if(oper->getId() != "=") framePtr->load(dst,"$t0",n1);
        //framePtr->load(dst,"$t1",n2);        
        
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
        else if (oper->getId() == "<") {
            dst << "slt $t2, $t0, $t1" << std::endl;
        }
        else if (oper->getId() == "<=") {
            dst << "slt $t2, $t0, $t1" << std::endl; // if t0<t1, t2 is 1

            dst << "xor $t3, $t0, $t1" << std::endl;
            dst << "sltu $t3, $t3, 1" << std::endl;     // if t0 == t1, t3 is 0 and slt makes t3 = 1

            dst << "or $t2, $t2, $t3" << std::endl;     // either one is true means <= is true
        }
        else if (oper->getId() == ">") {
            dst << "slt $t2, $t1, $t0" << std::endl;
        }
        else if (oper->getId() == ">=") {
            dst << "slt $t2, $t1, $t0" << std::endl; // if t0>t1, t1<t0 and thus t2 is 1

            dst << "xor $t3, $t0, $t1" << std::endl;
            dst << "sltu $t3, $t3, 1" << std::endl;     // if t0 == t1, t3 is 0 and slt makes t3 = 1

            dst << "or $t2, $t2, $t3" << std::endl;     // either one is true means >= is true
        }
        else if (oper->getId() == "<<") {
            dst << "sllv $t2, $t0, $t1" << std::endl;
        }
        else if (oper->getId() == ">>") {
            dst << "srav $t2, $t0, $t1" << std::endl;   // if unsigned we have to use srlv instead
        }
        else if(oper->getId() == "="){
            if(dynamic_cast<const PrimaryExpr*>(operand1)){
                std::string id = dynamic_cast<const PrimaryExpr*>(operand1)->getId();
                framePtr->store(dst, "$t1", id, type);
                dst << "move $t2, $t1\n";
            }else if(dynamic_cast<const PostfixExpr*>(operand1)){
                framePtr->storeArrayElement(dst, "$t1", dynamic_cast<const PostfixExpr*>(operand1));
                dst << "move $t2, $t1" << std::endl;
           }
        }

        framePtr->store(dst, "$t2", destName, type);
    }
    
    Type getType(const Frame* framePtr)const override{
        Type type1 = operand1->getType(framePtr);
        Type type2 = operand2->getType(framePtr);
        
        if(type1 == type2){
            return type1;
            
        }else{
            throw std::runtime_error("Called getType() on class BinaryOperation. Types didn't match (haven't impemented this)");
        }
        
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

#endif

#ifndef ast_statement_hpp
#define ast_statement_hpp

#include "ast_real/ast/statement.hpp"
#include "ast_real/compiler/typeConv.hpp"

class Frame;

class ExprStatement: public Statement{
private:
    const Expr* expr;    
public:
    ExprStatement(const Expr* _expr)
        :expr(_expr){}
        
    virtual void print(std::ostream& dst) const override{
        if(expr != NULL){
            expr->print(dst);
            dst << ";";
        }else{
            dst << ";";
        }
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
	    if(expr != NULL) expr->printPy(dst, depth);
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        if(expr != NULL){
            std::string destName = makeName();       
            expr->printMipsE(dst, destName, framePtr, Type::ANYTHING);
        }
    }
    
    void printMipsE(std::ostream& dst, std::string& destName, Frame* framePtr, Type type)const{
        if (expr != NULL) expr->printMipsE(dst, destName, framePtr, Type::ANYTHING);
    }
    
};

class JumpStatement: public Statement{
private:
    StrPtr str1;
    StrPtr str2;
    const Expr* p1;
    
public:
    JumpStatement(StrPtr _str1, StrPtr _str2, const Expr* _p1)
        :str1(_str1), str2(_str2), p1(_p1){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *str1 << " ";
        if(str2 != NULL) dst << *str2;
        if(p1 != NULL) p1->print(dst);
        dst << ";";
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{
        for(int i(0); i < depth; ++i) dst << "\t";
        dst << *str1 << " ";
        if(str2 != NULL) dst << *str2;
        if(p1 != NULL) p1->printPy(dst);
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        if(*str1 == "return"){
            if(p1 != NULL){
                //Request expression to evaluate itself
                std::string destName = makeName("RETURN");
                Type retType = framePtr->loadType("return");
                p1->printMipsE(dst, destName, framePtr, retType);

                //Return value should be in $f0 not $2($v0) if return type is float/double
                if(retType == Type::FLOAT || retType == Type::DOUBLE){
                    framePtr->load(dst, "$f0", destName);
                }
                else{
                    framePtr->load(dst, "$v0", destName);
                }

            }else{
                //Have to find out what is the specification for v0 when the function returns void
                //For now, I will return 0
                dst << "move $v0, $zero\n";
            }
            framePtr->clean(dst);
        }
    }
};

class ForStatement: public Statement{
private:
    const ExprStatement* state1;
    const ExprStatement* state2;
    const Expr* expr;
    NodePtr state3;
    
public:
    ForStatement(const ExprStatement* _state1, const ExprStatement* _state2, const Expr* _expr, NodePtr _state3)
        :state1(_state1), state2(_state2), expr(_expr), state3(_state3){}
        
    virtual void print(std::ostream& dst) const override{
        dst << "for(";
        state1->print(dst);
        state2->print(dst);
        if(expr != NULL){
            expr->print(dst);
        }
        dst << ")";
        state3->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{}
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        framePtr->newScope();
        dst << "###### START OF FOR LOOP ######\n";
        
        std::string COND = makeName("COND");
        std::string START = makeName("START");
        std::string condition = makeName("condition");
        std::string exprName = makeName();        
        
        state1->printMips(dst, framePtr);
        dst << "b $" << COND << std::endl;       
        dst << "nop" << std::endl;
        
        dst << "$" << START << ":" << std::endl;
        state3->printMips(dst, framePtr);
        
        if(expr != NULL) expr->printMipsE(dst, exprName, framePtr, Type::INT);
        
        dst << "$" << COND << ":" << std::endl;
        state2->printMipsE(dst, condition, framePtr, Type::INT);
        framePtr->load(dst, "$t0", condition);

        dst<<"bne $0, $t0, $" << START << std::endl;
        dst << "nop" << std::endl;
   
        dst << "###### END OF FOR LOOP ######";
        framePtr->deleteScope();
    }   
 
};

class DoStatement: public Statement{
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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{}
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        framePtr->newScope();
        dst << "###### START OF DO-WHILE LOOP ######\n";
        
        std::string START = makeName("START");
        std::string condition = makeName("condition");
        
        dst << "$" << START << ":" << std::endl;
        statement->printMips(dst, framePtr);
        
        expr->printMipsE(dst, condition, framePtr, Type::INT);
        framePtr->load(dst, "$t0", condition);
        dst<<"bne $0, $t0, $" << START << std::endl;
        dst << "nop" << std::endl;
        
        dst << "###### END OF WHILE LOOP ######";
        framePtr->deleteScope();
    }  
    
    
};

class WhileStatement: public Statement{
private:
    const Expr* expr;
    NodePtr statement;
    
public:
    WhileStatement(const Expr* _expr, NodePtr _statement)
        :expr(_expr), statement(_statement){}
        
        
    virtual void print(std::ostream& dst) const override{
        dst << "while(";
        expr->print(dst);
        dst << ")";
        statement->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        for(int i =0; i < depth; i++){
            dst << "\t" ;
        }
        dst<<"while (";
        expr -> printPy(dst);
        dst << "):\n";
        statement->printPy(dst,depth+1); 

    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        framePtr->newScope();
        dst << "###### START OF WHILE LOOP ######\n";
        std::string COND = makeName("COND");
        std::string START = makeName("START");
        std::string condition = makeName("condition");
        
        dst << "b $" << COND << std::endl;       
        dst << "nop" << std::endl;
        
        dst << "$" << START << ":" << std::endl;
        statement->printMips(dst, framePtr);
        
        dst << "$" << COND << ":" << std::endl;
        expr->printMipsE(dst, condition, framePtr, Type::INT);
        framePtr->load(dst, "$t0", condition);
        dst<<"bne $0, $t0, $" << START << std::endl;
        dst << "nop" << std::endl;
        
        dst << "###### END OF WHILE LOOP ######";
        framePtr->deleteScope();
    }    

};

class CaseStatement: public Statement{
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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{}

};

class IfStatement: public Statement{
private:
    const Expr* expr;
    NodePtr statement1;
    NodePtr statement2;
    
public:
    IfStatement(const Expr* _expr, NodePtr _statement1, NodePtr _statement2 = NULL)
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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        for(int i(0); i < depth; ++i){
            dst << "\t";
        }
        dst << "if(";
        expr->printPy(dst);
        dst << "):";
        dst << "\n";
        statement1->printPy(dst, depth+1);
        dst << "\n";
        if(statement2 != NULL){
            for(int i(0); i <depth; ++i){
                dst << "\t";
            }
            dst << "else:";
            dst << "\n";
            statement2->printPy(dst,depth+1);
       }
   }

     virtual void printMips(std::ostream& dst, Frame* framePtr, Type type = Type::ANYTHING)const override{
        framePtr->newScope();
        dst << "###### START OF IF STATEMENT ######\n";
        std::string destName = makeName();
        std::string ELSE = makeName(std::string("ELSE"));
        std::string END = makeName(std::string("END"));        
        expr->printMipsE(dst, destName, framePtr, Type::INT);
        
        framePtr->load(dst,"$t0" , destName);
        dst<<"beq $t0, $0, $"<< ELSE << std::endl;
        dst << "nop " << std::endl;

        statement1 ->printMips(dst, framePtr);
        dst << "b $" << END << std::endl ;
        dst <<"nop" << std::endl;

        dst << "$" << ELSE << ":" <<std::endl;        
        if(statement2 != NULL){
            statement2 ->printMips(dst, framePtr);
        }

        dst << "$" << END << ":" << std::endl;             
        
        dst << "###### END OF IF STATEMENT ######\n";
        framePtr->deleteScope();
     }

    
};

class LabeledStatement: public Statement{
private:
    StrPtr str1;
    NodePtr p1;
    NodePtr p2;
    
public:
    LabeledStatement(StrPtr _str1, NodePtr _p1, NodePtr _p2)
        :str1(_str1), p1(_p1), p2(_p2){}
        
    virtual void print(std::ostream& dst)const override{
        dst << *str1 << " ";
        if(p1 != NULL) p1->print(dst);
        dst << ":";
        p2->print(dst);
    }
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override{}
};

#endif

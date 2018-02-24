#ifndef ast_statement_hpp
#define ast_statement_hpp

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
        if(p1 != NULL) p1->print(dst);
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        if(*str1 == "return"){
            if(p1 != NULL){
                //Request expression to evaluate itself
                std::string destName = makeName();
                p1->printMipsE(dst, destName, framePtr);
                framePtr->load(dst, "$v0", destName);
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
    const Expr* expr1;
    const Expr* expr2;
    const Expr* expr3;
    NodePtr statement;
    
public:
    ForStatement(const Expr* _expr1, const Expr* _expr2, const Expr* _expr3, NodePtr _statement)
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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{}
    
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        framePtr->newScope();
        std::string COND = makeName("COND");
        std::string START = makeName("START");
        std::string e1 = makeName();
        std::string e2 = makeName();
        std::string e3 = makeName();        
        
        expr1->printMipsE(dst,e1,framePtr);
        dst << "b $" << COND << std::endl;       
        dst << "nop" << std::endl;
        
        dst << "$" << START << ":" << std::endl;
        statement->printMips(dst, framePtr);
        expr3->printMipsE(dst, e3, framePtr);
        
        dst << "$" << COND << ":" << std::endl;
        expr2->printMipsE(dst,e2, framePtr);
        framePtr->load(dst, "$t3", e2);

        dst<<"bne $0, $t3, $" << START << std::endl;
        dst << "nop" << std::endl;
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
};

class WhileStatement: public Statement{
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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
        for(int i =0; i < depth; i++){
            dst << "\t" ;
        }
        dst<<"while (";
        expr -> printPy(dst);
        dst << "):\n";
        statement->printPy(dst,depth+1); 

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
    
    virtual ~CaseStatement() override{
        delete expr;
        delete statement;
    }
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

     virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        framePtr->newScope();
        
        std::string destName = makeName();
        std::string ELSE = makeName(std::string("ELSE"));
        std::string END = makeName(std::string("END"));        
        expr->printMipsE(dst, destName, framePtr);
        
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
        
        framePtr->deleteScope();
     }

    
};

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
	    expr->printPy(dst, depth);
    }
    
    virtual void printMips(std::ostream& dst, Frame* framePtr = NULL)const override{
        std::string destName = makeName();        
        expr->printMipsE(dst,destName,framePtr);
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

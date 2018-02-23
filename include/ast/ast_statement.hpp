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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{}
    
    virtual ~ForStatement() override{
        delete expr1;
        delete expr2;
        delete expr3;
        delete statement;
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
};

class ExprStatement: public Statement{
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
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override{
	    expr->printPy(dst, depth);
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

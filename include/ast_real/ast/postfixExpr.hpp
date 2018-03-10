#ifndef POSTFIX_EXPR_HPP
#define POSTFIX_EXPR_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class PostfixExpr;

#include "ast_real/ast/stringNode.hpp"
#include "ast_real/compiler/frame.hpp"
#include "ast_real/ast/expr.hpp"

class PostfixExpr: public Expr{
private:
    const Expr* primaryExpr;
    StrPtr oper1;
    const std::vector<const Expr*>* argumentExprList;
    StrPtr oper2;
    
public:
    PostfixExpr(const Expr* _primaryExpr, StrPtr _oper1, const std::vector<const Expr*>* _argumentExprList, StrPtr _oper2);
        
    virtual void print(std::ostream& dst)const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0)const override;
    
    virtual void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const;
    
    void evaluateArgument(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const;
    
    virtual std::string getId()const override;
};

#endif

#ifndef EXPR_VECTOR_HPP
#define EXPR_VECTOR_HPP

#include <vector>
#include <iostream>

#include "ast_real/ast/expr.hpp"

class ExprVector: public Expr{
private:
    std::vector<const Expr*> exprVector;
    
public:
    ExprVector(const Expr* _expr);
    
    void pushBack(const Expr* _expr);
    
    void print(std::ostream& dst)const override;
    
    void printPy(std::ostream& dst, int depth = 0)const override;
    
    void printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const override;
    
    Type getType(const Frame* framePtr)const override;
    
    bool isIdentifier()const override;
    
    double eval()const override;
    
    std::string getId()const override;
};

#endif

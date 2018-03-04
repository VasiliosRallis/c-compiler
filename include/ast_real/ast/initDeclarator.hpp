#ifndef INIT_DECLARATOR_HPP
#define INIT_DECLARATOR_HPP

#include <string>
#include <vector>

class InitDeclarator;

#include "ast_real/ast/directDeclarator.hpp"
#include "ast_real/ast/expr.hpp"
#include "ast_real/ast/node.hpp"

class InitDeclarator : public Node{
private:
    const DirectDeclarator* directDeclarator;
    const Expr* asgnExpr;
    const std::vector<const Expr*>* exprList;
    
public:
    InitDeclarator(const DirectDeclarator* _directDeclarator, const Expr* _asgnExpr, const std::vector<const Expr*>* _exprList = NULL);
    
    virtual void print(std::ostream& dst) const override;
    
    virtual void printPy(std::ostream& dst, int depth = 0) const override;
    
    virtual void printMips(std::ostream& dst, Frame* framePtr)const override;
    
    void addGlobalMips(std::ostream& dst)const;
    
    std::string getId()const;
    
    void addGlobal()const;
};

#endif

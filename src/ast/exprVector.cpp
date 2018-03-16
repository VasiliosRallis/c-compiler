#include "ast_real/ast/exprVector.hpp"

#include "ast_real/compiler/frame.hpp"

ExprVector::ExprVector(const Expr* _expr){
    exprVector.push_back(_expr);
}

void ExprVector::pushBack(const Expr* _expr){
    exprVector.push_back(_expr);
}   


void ExprVector::print(std::ostream& dst)const{assert(0);}

void ExprVector::printPy(std::ostream& dst, int depth)const{
    exprVector.at(0)->printPy(dst, depth);
}

void ExprVector::printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const{
    for(auto i = exprVector.begin(); i != exprVector.end() - 1; ++i){
        (*i)->printMipsE(dst, makeName(), framePtr, type);
    }
    exprVector.back()->printMipsE(dst, destName, framePtr, type);   
}

Type ExprVector::getType(const Frame* framePtr)const{
    return exprVector.back()->getType(framePtr);
}

bool ExprVector::isIdentifier()const{
    return exprVector.back()->isIdentifier();
}

double ExprVector::eval()const{
    return exprVector.back()->eval();
}

std::string ExprVector::getId()const{
    return exprVector.back()->getId();
}

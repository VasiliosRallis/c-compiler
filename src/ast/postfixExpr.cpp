#include "ast_real/ast/postfixExpr.hpp"

PostfixExpr::PostfixExpr(const Expr* _primaryExpr, StrPtr _oper1, const std::vector<const Expr*>* _argumentExprList, StrPtr _oper2)
    :primaryExpr(_primaryExpr), oper1(_oper1), argumentExprList(_argumentExprList), oper2(_oper2){}
    
void PostfixExpr::print(std::ostream& dst)const{
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

void PostfixExpr::printPy(std::ostream& dst, int depth)const{
    primaryExpr->printPy(dst);
    if(oper1 != NULL){
        dst << *oper1;
    }
    if(argumentExprList != NULL){
        for(int i(0); i < argumentExprList->size(); ++i){
            argumentExprList->at(i)->printPy(dst);
            if(i < argumentExprList->size() - 1) dst << ",";
        }
    }
    if(oper2 != NULL){
        dst << *oper2;
    }
}

void PostfixExpr::printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr)const{
    //Check if is is a function call
    if(*oper1 == "("){
        framePtr->saveArguments(dst, argumentExprList);
        dst << "jal " << primaryExpr->getId() << std::endl;
        dst << "nop" << std::endl;
        
        framePtr->store(dst, "$v0", destName);
    }
    if(*oper1 == "["){
        //Trying to access element of an array;
        framePtr->loadArrayElement(dst, "$t0", primaryExpr->getId(), argumentExprList->at(0));
        framePtr->store(dst, "$t0", destName);
    }
}

void PostfixExpr::evaluateArgument(std::ostream& dst, const std::string& destName, Frame* framePtr)const{
    argumentExprList->at(0)->printMipsE(dst, destName, framePtr);
}

std::string PostfixExpr::getId()const{
    return primaryExpr->getId();
}


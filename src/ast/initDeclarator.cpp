#include "ast_real/ast/initDeclarator.hpp"

InitDeclarator::InitDeclarator(const DirectDeclarator* _directDeclarator, const Expr* _asgnExpr, const std::vector<const Expr*>* _exprList)
    :directDeclarator(_directDeclarator), asgnExpr(_asgnExpr), exprList(_exprList) {}

void InitDeclarator::print(std::ostream& dst)const{
    dst << directDeclarator->getId();
    dst << "=";      
    asgnExpr->print(dst);
}

void InitDeclarator::printPy(std::ostream& dst, int depth)const{
    for(int i(0); i < depth; ++i) dst << "\t";
    dst << directDeclarator->getId();
    dst << "=";
    asgnExpr->printPy(dst);
}

void InitDeclarator::printMips(std::ostream& dst, Frame* framePtr)const{
    if(asgnExpr != NULL){
        //Generate a unique name
        std::string destName = makeName();
        //Ask the expression to evaluate itself and store its value in the frame, with destName as it's identifier
        asgnExpr->printMipsE(dst, destName, framePtr);
        //Temporary store the identifier in $t1
        framePtr->load(dst, "$t0", destName);
        //Store it in the frame
        framePtr->store(dst, "$t0", directDeclarator->getId(), true);
    }
    if(exprList != NULL){
        framePtr->storeArray(dst, directDeclarator->getId(), exprList, true);
    }
}

void InitDeclarator::addGlobalMips(std::ostream& dst)const{
    dst << "\t.globl\t" << directDeclarator->getId() << std::endl;
    dst << "\t.data" << std::endl;
    dst << "\t.align 2" << std::endl;
    dst << "\t.size\t" << directDeclarator->getId() << ", 4" << std::endl;
    dst << directDeclarator->getId() << ":" << std::endl;
    dst << "\t.word\t";
    asgnExpr->printPy(dst);
    dst << std::endl;
    
    g_mips_var.push_back(directDeclarator->getId());
}

std::string InitDeclarator::getId()const{
    return directDeclarator->getId();
}

void InitDeclarator::addGlobal()const{
    g_variables.push_back(directDeclarator->getId());
}

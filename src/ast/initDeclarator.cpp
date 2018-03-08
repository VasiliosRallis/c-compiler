#include "ast_real/ast/initDeclarator.hpp"
#include <cassert>
#include <sstream>

extern std::vector<std::string> endPrint;

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

void InitDeclarator::printMips(std::ostream& dst, Frame* framePtr, Type type)const{
    if(asgnExpr != NULL){
        if(type == Type::INT){
            //Generate a unique name
            std::string destName = makeName();
            //Ask the expression to evaluate itself and store its value in the frame, with destName as it's identifier
            asgnExpr->printMipsE(dst, destName, framePtr);
            //Temporary store the identifier in $t1
            framePtr->load(dst, "$t0", destName);
            //Store it in the frame
            framePtr->store(dst, "$t0", directDeclarator->getId(), true);
            
        }else if(type == Type::FLOAT){
            //Generate label identifier
            std::string label = std::string("$" + makeName("FLOAT"));
            
            //Add the code that needs to be printed at the end of
            //the assembly
            endPrint.push_back("\t.rdata\n");
            endPrint.push_back("\t.align 2\n");
            endPrint.push_back(label + ":\n");
            
            //This might not word for some inputs (non deterministic)
            std::stringstream ss;
            asgnExpr->printPy(ss);
            endPrint.push_back("\t.float " + ss.str() + "\n");
            
            //Load the containts of the rdata into a register
            dst << "lui $t0, %hi(" << label << ")" << std::endl;
            dst << "lwc1 $f0, %lo(" << label << ")($t0)" << std::endl;
            
            //Store the variable in the frame
            framePtr->store(dst, "$f0", directDeclarator->getId(), true, type);
            
            
        }else{
            assert(0);
        }
        
    }
    if(exprList != NULL){
        framePtr->storeArray(dst, directDeclarator->getId(), exprList, true);
    }
}

void InitDeclarator::addGlobalMips(std::ostream& dst)const{
    if(asgnExpr != NULL){
        //Check if we are dealing with a pointer
        if(asgnExpr->isAddr() == true){
            dst << "\t.globl\t" << directDeclarator->getId() << std::endl;
            dst << "\t.align 2" << std::endl;
            dst << "\t.size\t" << directDeclarator->getId() << ", 4" << std::endl;
            dst << directDeclarator->getId() << ":" << std::endl;
            dst << "\t.word\t" << asgnExpr->getId() << std::endl;
        }else{     
            dst << "\t.globl\t" << directDeclarator->getId() << std::endl;
            dst << "\t.data" << std::endl;
            dst << "\t.align 2" << std::endl;
            dst << "\t.size\t" << directDeclarator->getId() << ", 4" << std::endl;
            dst << directDeclarator->getId() << ":" << std::endl;
            dst << "\t.word\t";
            asgnExpr->printPy(dst);
            dst << std::endl;
        }
        g_mips_var.push_back(directDeclarator->getId());
    }else if(exprList != NULL){
        dst << "\t.globl\t" << directDeclarator->getId() << std::endl;
        dst << "\t.data" << std::endl;
        dst << "\t.align 2" << std::endl;
        dst << "\t.size\t" << directDeclarator->getId() << ", " << exprList->size()*4<< std::endl;
        dst << directDeclarator->getId() << ":" << std::endl;
        for(int i(0); i < exprList->size(); ++i){
            dst << "\t.word\t";
            exprList->at(i)->printPy(dst);
            dst << std::endl;
        }
        g_mips_var.push_back(directDeclarator->getId());
    }
    else{
        assert(false);
    }
}

std::string InitDeclarator::getId()const{
    return directDeclarator->getId();
}

void InitDeclarator::addGlobal()const{
    g_variables.push_back(directDeclarator->getId());
}

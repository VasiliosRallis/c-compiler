#include "ast_real/ast/postfixExpr.hpp"
#include <typeinfo>

#include "ast_real/compiler/typeConv.hpp"

extern std::unordered_map<std::string, Type> function_type;

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

void PostfixExpr::printMipsE(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const{
    //Check if is is a function call
    if(*oper1 == "("){
        framePtr->storeRegisters(dst);
        
        framePtr->saveArguments(dst, argumentExprList);
        dst << "jal " << primaryExpr->getId() << std::endl;
        dst << "nop" << std::endl;
        
        framePtr->loadRegisters(dst);
        
        framePtr->store(dst, "$v0", destName, type);
    }
    if(*oper1 == "["){
        //Trying to access element of an array;
        framePtr->loadArrayElement(dst, "$t0", primaryExpr->getId(), argumentExprList->at(0));
        Type elementType = addrToType(primaryExpr->getType(framePtr));
        
        if(type == Type::INT){
            if(elementType == Type::INT || elementType == Type::CHAR){
                //Do nothing
                
            }else if(elementType == Type::FLOAT){
                dst << "mtc1 $t0, $f0" << std::endl;
                TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                
            }else{assert(0);} //Haven't implemented it yet
            
        }else{assert(0);} //Haven't implemented it yet
        
        framePtr->store(dst, "$t0", destName, addrToType(primaryExpr->getType(framePtr)));
    }
    else if(*oper1 == "++"){            
            primaryExpr->printMipsE(dst,destName,framePtr, type);
	        framePtr->load(dst, "$t0", destName);
	        framePtr->store(dst, "$t0", destName, type);
            dst << "addi $t0, $t0, 1" << std::endl;
            std::string id2 = primaryExpr->getId();     // Wondering if there are problems with this, "5++;" Not valid in C89
            framePtr->store(dst, "$t0", id2, type);
    }
    else if(*oper1 == "--"){            
            primaryExpr->printMipsE(dst,destName,framePtr, type);
	        framePtr->load(dst, "$t0", destName);
	        framePtr->store(dst, "$t0", destName, type);
            dst << "addi $t0, $t0, -1" << std::endl;
            std::string id2 = primaryExpr->getId();     // Wondering if there are problems with this, "5--;" Not valid in C89
            framePtr->store(dst, "$t0", id2, type);
    }
}

void PostfixExpr::evaluateArgument(std::ostream& dst, const std::string& destName, Frame* framePtr, Type type)const{
    argumentExprList->at(0)->printMipsE(dst, destName, framePtr, type);
}

std::string PostfixExpr::getId()const{
    return primaryExpr->getId();
}

Type PostfixExpr::getType(const Frame* framePtr)const{
    if(*oper1 == "("){
        try{
            return function_type.at(primaryExpr->getId());
        }catch(std::exception& e){
            throw std::runtime_error("Called getType() in class PostfixExpr. Couldn't find function id in function_type map");
        }
        
    }else{
        return primaryExpr->getType(framePtr);
    }
}

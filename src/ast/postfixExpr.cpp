#include "ast_real/ast/postfixExpr.hpp"
#include <typeinfo>

#include "ast_real/compiler/typeConv.hpp"

extern std::unordered_map<std::string, std::vector<Type> > function_type;
extern std::unordered_map<std::string, std::vector<Type> > function_decl;

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
        std::string id = primaryExpr->getId();
        
        //Store the registers that have to be saved between function calls (for now it's just the argument registers)
        framePtr->storeRegisters(dst);
        
        framePtr->saveArguments(dst, id, argumentExprList);
        
        if(function_decl.find(id) != function_decl.end()){
            dst << "lw $t0, %call16(" << id << ")($28)" << std::endl;
            dst << "jalr $t0" << std::endl;
            dst << "nop" << std::endl;
            
        }else{ 
            dst << "jal " << id << std::endl;
            dst << "nop" << std::endl;
            
        }
        
        //Loads the registers that have to be saved between function calls
        framePtr->loadRegisters(dst);
        
        Type rType;
        if(function_type.find(id) != function_type.end()){
            rType = (function_type.at(id)).at(0);
            
        }else if(function_decl.find(id) != function_decl.end()){
            rType = (function_decl.at(id)).at(0);
        
        //Should never happen    
        }else{assert(0);}
        
        if(rType == Type::INT || isAddr(rType) || rType == Type::CHAR){
            framePtr->store(dst, "$v0", destName, type);
            
        }else if(rType == Type::FLOAT){
            framePtr->store(dst, "$f0", destName, type);
        
        }else if(rType == Type::VOID){
            //Do nothing
        //Haven't implemented double yet    
        }else{assert(0);}
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
        try{return function_type.at(primaryExpr->getId()).at(0);}
            catch(std::exception& e){
                try{return function_decl.at(primaryExpr->getId()).at(0);}
                    catch(std::exception& e){
                        throw std::runtime_error("Called getType() in class PostfixExpr. Couldn't find function id in function_type map");
                    }
            }    
    }else{
        return primaryExpr->getType(framePtr);
    }
}

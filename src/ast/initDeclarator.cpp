#include "ast_real/ast/initDeclarator.hpp"
#include <cassert>
#include <sstream>

extern bool isAddr(const Type type);
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
        Type myType = asgnExpr->getType(framePtr);
        
        if(type == Type::INT || type == Type::CHAR || isAddr(type)){
            //Generate a unique name
            std::string destName = makeName("init");
            //Ask the expression to evaluate itself and store its value in the frame, with destName as it's identifier
                      
            if(myType == Type::FLOAT){
                asgnExpr->printMipsE(dst, destName, framePtr, myType);
                framePtr->load(dst, "$f0", destName);
                TypeConv::convert(dst, Type::INT, Type::FLOAT, "$t0", "$f0");
                framePtr->store(dst, "$t0", directDeclarator->getId(), type, true); 
            }
            else{
                asgnExpr->printMipsE(dst, destName, framePtr, /*destType*/ type); // If we put destType which should be the answer, char and pointers get fked
                //Temporary store the identifier in $t0
                framePtr->load(dst, "$t0", destName);
                //Store it in the frame
                framePtr->store(dst, "$t0", directDeclarator->getId(), type, true);         
            }
            
        }else if(type == Type::FLOAT){
            //We have to check if the Asgn Expr is a constant and only perform this if its constant
            //Generate label identifier
            if(!(asgnExpr->isIdentifier())){
                std::string label = std::string("$" + makeName("FLOAT"));
                
                //Add the code that needs to be printed at the end of
                //the assembly
                endPrint.push_back("\t.rdata\n");
                endPrint.push_back("\t.align 2\n");
                endPrint.push_back(label + ":\n");
                
                //This might not word for some inputs (non deterministic)
                endPrint.push_back("\t.float " + std::to_string((float)asgnExpr->eval()) + "\n");
                
                //Load the containts of the rdata into a register
                dst << "lui $t0, %hi(" << label << ")" << std::endl;
                dst << "lwc1 $f0, %lo(" << label << ")($t0)" << std::endl;
                
                //Store the variable in the frame
                framePtr->store(dst, "$f0", directDeclarator->getId(), type, true);
            }
            else{
                //EVAL EXPR AND store into LHS Declarator
                std::string destName = makeName();
                if(myType == Type::INT || myType == Type::CHAR || isAddr(myType)){ // USED TO HAVE My type == int here
                     asgnExpr->printMipsE(dst, destName, framePtr, myType);
                    //Temporary store the identifier in $t0
                    framePtr->load(dst, "$t0", destName);
                    TypeConv::convert(dst, Type::FLOAT, Type::INT, "$f0", "$t0");
                    //Store it in the frame
                    framePtr->store(dst, "$f0", directDeclarator->getId(), type, true);            
                }
                else{           
                    //Ask the expression to evaluate itself and store its value in the frame, with destName as it's identifier
                    asgnExpr->printMipsE(dst, destName, framePtr, myType);
                    //Temporary store the identifier in $f0
                    framePtr->load(dst, "$f0", destName);
                    //Store it in the frame
                    framePtr->store(dst, "$f0", directDeclarator->getId(), type, true);       
                }

            }
        }
        else{
            assert(0);
        }
        
    }
    if(exprList != NULL){
        if(!isAddr(type)){
            //This should never happen
            assert(0);
   
        }else{
            framePtr->storeArray(dst, directDeclarator->getId(), exprList, type, directDeclarator->getSize(), true);
            
        }
    }
}

std::string InitDeclarator::getId()const{
    return directDeclarator->getId();
}

void InitDeclarator::addGlobal()const{
    g_variables.push_back(directDeclarator->getId());
}

void InitDeclarator::printGMips(std::ostream& dst, Type type)const{
    std::string id = directDeclarator->getId();
    if(asgnExpr != NULL){
        Type myType = asgnExpr->getType(NULL);

        dst << "\t.globl\t" << id << std::endl;
        
        if(type == Type::INT){
            dst << "\t.align 2" << std::endl;
            dst << "\t.data" << std::endl;
            dst << "\t.size\t" << id << ", 4" << std::endl;
            dst << id << ":" << std::endl;
            
            if(myType == Type::INT || myType == Type::FLOAT || myType == Type::CHAR){
                dst << "\t.word\t" <<  (int)asgnExpr->eval() << std::endl;
            
            }
            else{
                assert(0); //Should never happen (myType cannot be Addr)
            }
        }else if(isAddr(type)){
            dst << "\t.globl\t" << id << std::endl;
            dst << "\t.section	.data.rel.local,\"aw\",@progbits" << std::endl;
            dst << "\t.align 2" << std::endl;
            dst << "\t.size\t" << id << ", 4" << std::endl;
            dst << id << ":" << std::endl;
       
            if(isAddr(myType)){
                dst << "\t.word\t" << asgnExpr->getId() << std::endl;
                
            }else if(myType == Type::STRING){
                std::string label = makeName("label");
                dst << "\t.word\t" << label << std::endl;
                
                dst << "\t.rdata\t" << std::endl;
                dst << "\t.align 2" << std::endl;
                dst << label << ":" << std::endl;
                std::string myString = asgnExpr->getString();
                myString.erase(myString.begin());
                myString.erase(myString.end() - 1);
                dst << "\t.ascii\t\"" << myString << "\\000\"" << std::endl;
                
            }else if(myType == Type::INT || myType == Type::CHAR){
                dst << "\t.word\t" <<  (int)asgnExpr->eval() << std::endl;
                
            }else{assert(0);} //Should never happen (myType cannot be float)
            
        }else if(type == Type::FLOAT){
            dst << "\t.align 2" << std::endl;
            dst << "\t.data" << std::endl;
            dst << "\t.size\t" << id << ", 4" << std::endl;
            dst << id << ":" << std::endl;
            
            if(myType == Type::INT || myType == Type::CHAR || myType == Type::FLOAT){
                dst << "\t.float\t" << asgnExpr->eval() << std::endl;
                
            }else{assert(0);} //Should never happen (myType cannot be addr)
        
        }else if(type == Type::CHAR){
            dst << "\t.data" << std::endl;
            dst << "\t.size\t" << id << ", 1" << std::endl;
            dst << id << ":" << std::endl;
            
            if(myType == Type::INT || myType == Type::CHAR){
                dst << "\t.byte\t" << (int)(unsigned char)asgnExpr->eval() << std::endl;
                
            }else if(myType == Type::FLOAT){
                int c = (int)asgnExpr->eval();
                if(c > 127) c = 127;
                if(c < -128) c = -128;
                dst << "\t.byte\t" << (int)c << std::endl;
                
            }else{assert(0);} //Should never happen (myType cannot be addr)
            
        }else{assert(0);} //All types covered should never happen
        
    }else if(exprList != NULL){
        if(!isAddr(type)){
            assert(0); //Should never happen
            
        }else{
            std::string p_to_array = makeName("pointer");
            dst << "\t.globl\t" << p_to_array<< std::endl;
            dst << "\t.data" << std::endl;
            dst << "\t.align 2" << std::endl;
            
            
            if(type == Type::INT_ADDR || type == Type::CHAR_ADDR || type == Type::FLOAT_ADDR){
                int mySize;
                int objSize;
                
                if(directDeclarator->getSize() > exprList->size()){
                    mySize = directDeclarator->getSize();
                    
                }else{
                    mySize = exprList->size();
                    
                }
                
                if(type == Type::INT_ADDR || type == Type::FLOAT_ADDR){
                    objSize = 4;
                    
                }else{
                    objSize = 1;
                    
                }
                
                dst << "\t.size\t" << p_to_array << ", " << mySize*objSize << std::endl;
                dst << p_to_array << ":" << std::endl;
                
                if(mySize > exprList->size()){
                    dst << "\t.space\t" << objSize*(mySize-exprList->size()) << std::endl;
                }
                    
                for(int i(0); i < exprList->size(); ++i){
                    
                    if(type == Type::INT_ADDR){
                        //This will automatically truncate
                        int constant = exprList->at(i)->eval();
                        dst << "\t.word\t" << constant << std::endl;   
                    
                    }else if(type == Type::CHAR_ADDR){
                        char constant = exprList->at(i)->eval();
                        dst << "\t.byte\t" << (int)constant << std::endl;
                           
                    }else if(type == Type::FLOAT_ADDR){
                        float constant = exprList->at(i)->eval();
                        dst << "\t.float\t" << constant << std::endl;
                    
                    }else{assert(0);}
                        
                      
                }
                
                dst << "\t.globl\t" << id << std::endl;
                dst << "\t.section	.data.rel.local,\"aw\",@progbits" << std::endl;
                dst << "\t.align 2" << std::endl;
                dst << "\t.size\t" << id << ", 4" << std::endl;
                dst << id << ":" << std::endl;
                dst << "\t.word\t" << p_to_array << std::endl;
                    
            }else{assert(0);}
        }
    }
    
    g_mips_var.insert({id, type});   
}

bool InitDeclarator::isPointer()const{
    return directDeclarator->isPointer();
}       

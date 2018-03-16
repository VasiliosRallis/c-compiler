#include "ast_real/compiler/argTranslator.hpp"

ArgTranslator::ArgTranslator(const DirectDeclarator* directDeclarator){
    const std::vector<const ParameterDeclaration*>* v1 = directDeclarator->getParameterList();
    if(v1 != NULL){
    
        bool integralHit(false);
        int freeIntegralWords = 4;
        int freeFPWords = 4;
        
        for(int i(0); i < v1->size(); ++i){
            Type myType = v1->at(i)->getType(NULL);
            std::string id = v1->at(i)->getId();
            argTypeMap.insert({id, myType});
        
            if(myType == Type::INT || myType == Type::CHAR || isAddr(myType)){
                integralHit = true;
      
                if(freeIntegralWords > 0){
                    argMap.insert({id, std::string("$a").append(std::to_string(4 - freeIntegralWords))});
                    --freeIntegralWords;
                    
                }else{
                    argMap.insert({id, std::to_string(12 + 4*i)});
                    
                }
                
            }else if(myType == Type::FLOAT){
                if(integralHit){
                    if(freeIntegralWords > 0){
                        argMap.insert({id, std::string("$a").append(std::to_string(4 - freeIntegralWords))});
                        --freeIntegralWords;
                         
                    }else{
                        argMap.insert({id, std::to_string(12 + 4*i)});
                        
                    }
                //Not integralHit    
                }else{
                    if(freeFPWords > 0){
                        argMap.insert({id, std::string("$f").append(std::to_string(16 - freeFPWords))});
                        --freeIntegralWords; //Just following ABI
                        freeFPWords = freeFPWords - 2;
                    
                    }else if(freeIntegralWords > 0){
                        argMap.insert({id, std::string("$a").append(std::to_string(4 - freeIntegralWords))});
                        --freeIntegralWords;
                        
                    }else{
                        argMap.insert({id, std::to_string(12 + 4*i)});
                        
                    }
                }
            }else if(myType == Type::DOUBLE){
                assert(0);
           }else{assert(0);}//Haven't implemented yet
  
            //if(i < 4) argMap.insert({v1->at(i)->getId(), std::string("$a").append(std::to_string(i))});
            //Store the address of the argument in the statck relative to the previous $sp
            //else argMap.insert({v1->at(i)->getId(), std::to_string(4 * i + 12)});
        }
    }
}


void ArgTranslator::load(std::ostream& dst, const std::string& destName, const std::string& varName)const{
    if(argTypeMap.at(varName) == Type::FLOAT){
        if(argMap.at(varName).at(0) == '$'){
            if(destName[1] == 'f' && destName[2] != 'p'){
                if(argMap.at(varName).at(1) == 'f' && argMap.at(varName).at(2) != 'p'){
                    dst << "mov.s " << destName << ", " << argMap.at(varName) << std::endl;
                
                }else{
                   dst << "mtc1 " << argMap.at(varName) << ", " <<  destName<< std::endl;
                }
                   
            }else{
                if(argMap.at(varName).at(1) == 'f' && argMap.at(varName).at(2) != 'p'){
                    dst << "mfc1 " << destName << ", " << argMap.at(varName) << std::endl;
                
                }else{
                   dst << "move " << destName << ", " << argMap.at(varName) << std::endl;
                }
            }
        
        //The argument is in the frame    
        }else{
            if(destName[1] == 'f' && destName[2] != 'p'){
                dst << "lwc1 " << destName << ", " << argMap.at(varName) << std::endl;
                
            }else{
                dst << "lw " << destName << ", " << argMap.at(varName) << "($fp)" << std::endl;
            }
        }      
    }else if(argTypeMap.at(varName) == Type::INT || isAddr(argTypeMap.at(varName)) || argTypeMap.at(varName) == Type::CHAR){
        if(argMap.at(varName).at(0) == '$'){
            if(destName[1] == 'f' && destName[2] != 'p'){
                dst << "mtc1 " << argMap.at(varName) << ", " << destName << std::endl;
                
            }else{
                dst << "move " << destName << ", " << argMap.at(varName) << std::endl;
                
            }
        }else{
            if(destName[1] == 'f' && destName[2] != 'p'){
                dst << "lwc1 " << destName << ", " << argMap.at(varName)  << "($fp)" << std::endl;
                
            }else{
                dst << "lw " << destName << ", " << argMap.at(varName) << "($fp)" << std::endl;
                
            }
        }
        
    }else{assert(0);} //Haven't implemented DOUBLE
}

Type ArgTranslator::loadType(const std::string& id)const{
    return argTypeMap.at(id);
}

#include "ast_real/compiler/frame.hpp"
#include "cassert"

extern std::unordered_map<std::string, std::vector<Type> > function_type;
extern std::unordered_map<std::string, std::vector<Type> > function_decl;

void Frame::addWords(std::ostream& dst, int n){
    int bytes = n*4;
    dst << "addiu $sp, $sp, -" << bytes << "\n";
    freeWords += n;
}

Frame::Frame(std::ostream& dst, const DirectDeclarator* directDeclarator)
    :argTranslator(new ArgTranslator(directDeclarator)){

    std::unordered_map<std::string, int> temp1;
    scopeMap.push_back(temp1);
    
    std::unordered_map<std::string, Type> temp2;
    typeMap.push_back(temp2);
    
    //Allocate the minumum frame
    scopeMap.front().insert({"oldFramePointer", 8});
    scopeMap.front().insert({"returnAddr", 4});
    
    
    freeWords = 1;
    nextFreeAddr = 0;
    
    dst << "addiu $sp, $sp, -12\n";
    dst << "sw $fp, 8($sp)\n";
    dst << "sw $31, 4($sp)\n";
    dst << "move $fp, $sp" << std::endl;
}

void Frame::load(std::ostream& dst, const std::string reg, const std::string varName)const{
    try{
        if(reg[1] == 'f' && reg[2] != 'p'){
            if(loadType(varName) == Type::FLOAT || loadType(varName) == Type::INT ){   
                dst << "lwc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
                dst <<"nop" << std::endl;
            
            }else if(loadType(varName) == Type::DOUBLE){
                 dst << "lwc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
                 dst << "nop" << std::endl;
                 
                //This will give the register number
                int a = std::atoi(reg.substr(2).c_str());
                ++a;
                dst << "lwc1 " << std::string(reg.substr(0,2).append(std::to_string(a))) << ", " << scopeMap.back().at(varName)- 4  << "($fp)" << std::endl;
                dst << "nop" << std::endl;
                
            }else{assert(0);}   
                 
        }else{
            if(typeMap.back().find(varName) != typeMap.back().end()){
                if(typeMap.back().at(varName) == Type::CHAR){
                    dst << "lbu " << reg << ", " << scopeMap.back().at(varName) + 3 << "($fp)" << std::endl;
                }else{
                     dst << "lw " << reg << ", " << scopeMap.back().at(varName) << "($fp)" << std::endl;
                }
            }else{
                dst << "lw " << reg << ", " << scopeMap.back().at(varName) << "($fp)" << std::endl;
            }
            
        } 
    }catch(const std::out_of_range& e){
        try{
            argTranslator->load(dst, reg, varName);
        }catch(const std::out_of_range& e){
            if(g_mips_var.find(varName) != g_mips_var.end()){
                
                //Check if we are loading a global floating point
                if(reg[1] == 'f' && reg[2] != 'p'){    
                    //TODO never use $t9 again
                    dst << "lui $t9 " << ",%hi(" << varName << ")" << std::endl;
                    dst << "lwc1 " << reg <<  ",%lo(" << varName << ")($t9)" << std::endl;
                    dst << "nop" << std::endl;
                    
                }else{
                    dst << "lui " << reg << ",%hi(" << varName << ")" << std::endl;
                    dst << "lw " << reg <<  ",%lo(" << varName << ")(" << reg << ")" << std::endl;
                }
                
            }else{
                throw(std::runtime_error("Couldn't find " + varName + " in Frame::load"));
            }
        }      
    }
}

void Frame::store(std::ostream& dst, const std::string reg, const std::string varName, Type type, bool force){
    //This is a tricky case needs care
    if(force){
        if(scopeMap.back().find(varName) != scopeMap.back().end()){
            scopeMap.back().erase(varName);
            scopeMap.back().insert({varName, nextFreeAddr});
            typeMap.back().erase(varName);
            typeMap.back().insert({varName, type});

        }else{
            if(freeWords == 1) addWords(dst, scopeMap.back().size());
            scopeMap.back().insert({varName, nextFreeAddr});
            typeMap.back().insert({varName, type});
            nextFreeAddr -= 4;
            freeWords--;
        }
            
        //Check if we are storing a floating point register
        if(reg[1] == 'f' && reg[2] != 'p'){
        
            if(type == Type::FLOAT){
                dst << "swc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
                
            }else if(type == Type::DOUBLE){
             
                dst << "swc1 " << reg << ", " << scopeMap.back().at(varName)  << "($fp)" << std::endl;
                dst << "nop" << std::endl;
                
                //This will give the register number
                int a = std::atoi(reg.substr(2).c_str());
                ++a;
                dst << "swc1 " << std::string(reg.substr(0,2).append(std::to_string(a))) << ", "<< scopeMap.back().at(varName) - 4 << "($fp)" << std::endl;
                dst << "nop" << std::endl;
            
            //Should not happen
            }else{assert(0);}

        }else{
            if(type == Type::CHAR){
                dst << "sb " << reg << ", " << scopeMap.back().at(varName) + 3 << "($fp)" << std::endl;
                
            }else{
                dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)" << std::endl;
            }
 
        }
    }else{
        if(scopeMap.back().find(varName) != scopeMap.back().end()){
            //This code will be fixed    
            if(reg[1] == 'f' && reg[2] != 'p'){
                dst << "swc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
                dst << "nop" << std::endl;
                
            }else{
                if(type == Type::CHAR){
                    dst << "sb " << reg << ", " << scopeMap.back().at(varName) + 3 << "($fp)" << std::endl;
                
                }else{
                    dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)" << std::endl;
                }
            }
            
        }else if(g_mips_var.find(varName) != g_mips_var.end()){
            dst << "lui $t7, %hi(" << varName << ")" << std::endl;
            dst << "sw " << reg << ",%lo(" << varName << ")($t7)" << std::endl;
            
            if(type == Type::CHAR){
                dst << "sw " << reg << ",%lo(" << varName << ")($t7)" << std::endl;
                
            }else{
                dst << "sw " << reg << ",%lo(" << varName << ")($t7)" << std::endl;
            }
            
        }else{
            if(freeWords == 0) addWords(dst, scopeMap.back().size());
            scopeMap.back().insert({varName, nextFreeAddr});
            typeMap.back().insert({varName, type});
            nextFreeAddr -= 4;
            freeWords--;
            
            //Check if we are storing a floating point register
            if(reg[1] == 'f' && reg[2] != 'p'){
                dst << "swc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)" << std::endl;
                dst << "nop" << std::endl;                
            }else{
                if(type == Type::CHAR){
                    dst << "sb " << reg << ", " << scopeMap.back().at(varName) + 3 << "($fp)" << std::endl;
                
                }else{
                    dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)" << std::endl;
                }
                
            }
        }
        
    }
}

void Frame::newScope(){
    scopeMap.push_back(scopeMap.back());
    typeMap.push_back(typeMap.back());
}

void Frame::deleteScope(){
    scopeMap.pop_back();
    typeMap.pop_back();
}
   
void Frame::clean(std::ostream& dst)const{
    dst << "move $sp, $fp\n";
    load(dst, "$31", "returnAddr");
    load(dst, "$fp", "oldFramePointer");
    dst << "addiu $sp, $sp, +12\n";
    dst << "jr $31\n";
    dst << "nop\n";
}

void Frame::saveArguments(std::ostream& dst, const std::string& f_Id, const std::vector<const Expr*>* argumentExprList){
    //Looks compilcated but we need to do it this way so that the $sp doesn't change
    if(argumentExprList != NULL){
        dst << "###### Storing Arguments ######" << std::endl;
        
        std::vector<Type> argTypes;
        if(function_type.find(f_Id) != function_type.end()){
            argTypes = function_type.at(f_Id);
            
        }else if(function_decl.find(f_Id) != function_decl.end()){
            argTypes = function_decl.at(f_Id);
            
        }else{assert(0);} //Should never happen
        
        //Remove the first type which is the return type
        argTypes.erase(argTypes.begin());
        
        //Vector to hold the names of the Variables of the expressions
        std::vector<std::string> argumentNames;
        
        //Calculate the arguments
        for(int i(0); i < argumentExprList->size(); ++i){
            std::string exprName = makeName("arg");
            argumentNames.push_back(exprName);

            argumentExprList->at(i)->printMipsE(dst, argumentNames.back(), this, argTypes.at(i));
                
        }
        
        //Leave space for incoming arguments
        addWords(dst, argumentExprList->size());
        nextFreeAddr -= 4 * argumentExprList->size();
        freeWords = 0;
        
        bool integralHit(false);
        int freeIntegralWords = 4;
        int freeFPWords = 4;
        
        //From this point $sp can't change!!!
        for(int i(0); i < argumentNames.size(); ++i){
            if(argTypes.at(i) == Type::INT || argTypes.at(i) == Type::CHAR || isAddr(argTypes.at(i))){
                integralHit = true;
                
                if(freeIntegralWords > 0){
                    load(dst, std::string("$a").append(std::to_string(4 - freeIntegralWords)), argumentNames.at(i));
                    --freeIntegralWords;
                    
                }else{
                    load(dst, "$t0", argumentNames.at(i));
                    dst << "sw $t0, " << 4*i << "($sp)" << std::endl;
                    
                }
                
            }else if(argTypes.at(i) == Type::FLOAT){
                if(integralHit){
                    if(freeIntegralWords > 0){
                        load(dst, std::string("$a").append(std::to_string(4 - freeIntegralWords)), argumentNames.at(i));
                        --freeIntegralWords;
                         
                    }else{
                        load(dst, "$t0", argumentNames.at(i));
                        dst << "sw $t0, " << 4*i << "($sp)" << std::endl;
                        
                    }
                //Not integralHit    
                }else{
                    if(freeFPWords > 0){
                        load(dst, std::string("$f").append(std::to_string(16 - freeFPWords)), argumentNames.at(i));
                        --freeIntegralWords; //Just following ABI
                        freeFPWords = freeFPWords - 2;
                    
                    }else if(freeIntegralWords > 0){
                        load(dst, std::string("$a").append(std::to_string(4 - freeIntegralWords)), argumentNames.at(i));
                        --freeIntegralWords;
                        
                    }else{
                        load(dst, "$t0", argumentNames.at(i));
                        dst << "sw $t0, " << 4*i << "($sp)" << std::endl;
                        
                    }
                }
            }else if(argTypes.at(i) == Type::DOUBLE){
                assert(0);
           }else{assert(0);}//Haven't implemented yet
        }
    }
}


void Frame::storeArray(std::ostream& dst, const std::string& arrayName, const std::vector<const Expr*>* argumentExprList, const Type type, bool force){
    if(argumentExprList != NULL){
        
        std::vector<std::string> elementName;
        Type elementType = addrToType(type);
        
        //Calculate the value of all the elements
        dst << "###### CALCULATING ARRAY ELEMENTS ######" << std::endl;
        for(int i(0); i < argumentExprList->size(); ++i){
            elementName.push_back(makeName());
            argumentExprList->at(i)->printMipsE(dst, elementName.back(), this, elementType);
        }   
        dst << "###### FINISHED CALCULATING ELEMENTS ######" << std::endl;
        
        dst << "###### MAKING SPACE FOR ARRAY ######" << std::endl;
        //Allocate enough space for the array (and for the pointer to the array)
        while(freeWords < argumentExprList->size() + 1) addWords(dst, scopeMap.back().size());
        
        if(force){
            if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
                scopeMap.back().erase(arrayName);
                scopeMap.back().insert({arrayName, nextFreeAddr - 4 * (freeWords - 1)});
                typeMap.back().erase(arrayName);
                typeMap.back().insert({arrayName, type});

            }else{
                scopeMap.back().insert({arrayName, nextFreeAddr - 4 * (freeWords - 1)});
                typeMap.back().insert({arrayName, type});
            }
            
            nextFreeAddr -= 4;
            freeWords--;
                
        }else{
            if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
                
                //Do nothing (everything is do further down)
                
            }else{assert(0);}    
        }
        
        dst << "###### CALCULATING ADDRESS OF ARRAY ######" << std::endl;
        //The last element in the stack is going to be the pointer to the array
        dst << "addi $t0, $sp, 4" << std::endl;
        dst << "sw $t0, 0($sp)" << std::endl;
        
        dst << "###### STORING ARRAY ELEMENTS ######" << std::endl;
        //Store the elements of the array
        int byteIndex = 0;
        for(int i(0); i < argumentExprList->size(); ++i){
            if(elementType == Type::INT || elementType == Type::FLOAT){
                load(dst, "$t0", elementName.at(i));
                
                //The element of the array is lost we can only access it with the pointer
                //store(dst, "$t0", makeName(), elementType);
                
                dst << "sw $t0, " << byteIndex + 4 << "($sp)" << std::endl;
                byteIndex += 4;
            
            //Special case have to store as bytes
            }else if(elementType == Type::CHAR){
                load(dst, "$t0", elementName.at(i));
                
                dst << "sb $t0, " << byteIndex + 4 << "($sp)" << std::endl;
                ++byteIndex;
            
            }else{assert(0);}
            
        }
        dst << "###### FINISHED STORING ARRAY ELEMENTS ######" << std::endl;
        
        //Reset the stack
        nextFreeAddr -= ((freeWords - 1) * 4) + 4;
        freeWords = 0;
        
    }else{assert(0);}
}

void Frame::loadArrayElement(std::ostream& dst, const std::string& reg, const std::string& arrayName, const Expr* index){
    dst << "######## LOADING ARRAY ELEMENT ########" << std::endl;
    
    Type elementType = addrToType(loadType(arrayName));
    
    //Calculate the index
    std::string indexName = makeName();
    index->printMipsE(dst, indexName, this, Type::INT);
    load(dst, "$t0", indexName);

    if(elementType == Type::INT || elementType == Type::FLOAT){
        dst << "sll $t0, $t0, 2" << std::endl;
        
    }else if(elementType == Type::DOUBLE){
        dst << "sll $t0, $t0, 3" << std::endl;
        
    }else if(elementType == Type::CHAR){
        //Do nothing
        
    }else{assert(0);}
    
    //Check if array is in frame
    if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
        //Calculate the pointer to the element
        load(dst, "$t1", arrayName);
        dst << "addu $t1, $t1, $t0" << std::endl;
    
        //Load the element
        if(elementType == Type::INT || elementType == Type::FLOAT){
            dst << "lw " << reg << ", 0($t1)" << std::endl;
            
        }else if(elementType == Type::CHAR){
            dst << "lb " << reg << ", 0($t1)" << std::endl;
            
        }else{assert(0);} //Haven't implemented it yet
        
    }else{
        try{
            argTranslator->load(dst, "$t1", arrayName);
            dst << "addu $t1, $t1, $t0" << std::endl;
            
            //Load the element
            if(elementType == Type::INT || elementType == Type::FLOAT){
                dst << "lw " << reg << ", 0($t1)" << std::endl;
                
            }else if(elementType == Type::CHAR){
                dst << "lb " << reg << ", 0($t1)" << std::endl;
                
            }else{assert(0);} //Haven't implemented it yet
            
                
        }catch(const std::out_of_range& e){
            //Array is global
            dst << "lui $t1, %hi(" << arrayName << ")" << std::endl;
            dst << "addiu $t1, $t1, %lo(" << arrayName << ")" << std::endl;
            dst << "addu $t1, $t1, $t0" << std::endl;
            dst << "lw " << reg << ", " << "0($t1)" << std::endl;
        }
    }
    
    dst << "######## DONE LOADING ########" << std::endl;
}

void Frame::storeArrayElement(std::ostream& dst, const std::string& reg, const PostfixExpr* postfixExpr){
    dst << "######## STORING ARRAY ELEMENT ########" << std::endl;
    
    //We have to store so that we don't overwright it
    std::string safetyStore(makeName());
    store(dst, reg, safetyStore, Type::INT);
    
    //Get the identifier of the array
    std::string arrayName = postfixExpr->getId();
    
    Type elementType = addrToType(loadType(arrayName));
    
    //Evaluate the index of the array element
    std::string indexName = makeName();
    postfixExpr->evaluateArgument(dst, indexName, this, Type::INT);
    load(dst, "$t0", indexName);
    
    if(elementType == Type::INT || elementType == Type::FLOAT){
        dst << "sll $t0, $t0, 2" << std::endl;
        
    }else if(elementType == Type::DOUBLE){
        dst << "sll $t0, $t0, 3" << std::endl;
        
    }else if(elementType == Type::CHAR){
        //Do nothing
        
    }else{assert(0);}
        
    //Check if the array is in local scope
    if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
        //Load the pointer to the array
        load(dst, "$t1", arrayName);
        
        dst << "addu $t0, $t1, $t0" << std::endl;
        load(dst, "$t1", safetyStore);
        dst << "sw $t1, 0($t0)" << std::endl;
    }
    else{
        //Find the address of the array
        dst << "lui $t1, %hi(" << arrayName << ")" << std::endl;
        dst << "addiu $t1, $t1, %lo(" << arrayName << ")" << std::endl;
        
        //Find the address of the element
        dst << "addu $t0, $t1, $t0" << std::endl;
                
        //Store
        load(dst, "$t1", safetyStore);
        dst << "sw $t1, 0($t0)" << std::endl;
    }
    
    dst << "######## DONE STORING ########" << std::endl;
}

void Frame::loadAddr(std::ostream& dst, const std::string& reg, const std::string& varName)const{
    //Check if the variable is in local scope
    if(scopeMap.back().find(varName) != scopeMap.back().end()){
        Type myType = loadType(varName);
        
        if(myType == Type::CHAR){
            dst << "addi " << reg << ", $fp, " << scopeMap.back().at(varName) + 3<< std::endl;
        
        }else{
            dst << "addi " << reg << ", $fp, " << scopeMap.back().at(varName) << std::endl;
            
        }
        
    }else{
        try{
            argTranslator->load(dst, reg, varName);
            
        }catch(const std::out_of_range& e){
            //Assuming that it is in Global
            
            //Find the address of the variable
            dst << "lui " << reg << ", %hi(" << varName << ")" << std::endl;
            dst << "addiu " << reg << ", " << reg << ", %lo(" << varName << ")" << std::endl;
        }
    }       
}

void Frame::storeType(const std::string& id, const Type type){
    //Check if it is already in the current type scope
    if(typeMap.back().find(id) != typeMap.back().end()){
        throw std::runtime_error("In Frame::storeType, tried to store " + id + " which is already in use");
    }else{
        typeMap.back().insert({id,type});
    }
}

Type Frame::loadType(const std::string& id)const{
    try{return typeMap.back().at(id);
        
    }catch(const std::out_of_range& e){
        try{return argTranslator->loadType(id);
            
        }catch(const std::out_of_range& e){
            try{return g_mips_var.at(id);
                
            }catch(const std::out_of_range& e){
                throw std::runtime_error("In Frame::loadType, couldn't find " + id);
            }
        }
    }
}

void Frame::storeRegisters(std::ostream& dst){
    dst << "###### STORING REGISTERS ######" << std::endl;
    for(int i(0); i <= 3; ++i){
        std::string regName = "$a" + std::to_string(i);
        store(dst, regName, regName, Type::INT);
    }
    dst << "###### DONE STORING REGISTERS ######" << std::endl;
}

void Frame::loadRegisters(std::ostream& dst)const{
    dst << "###### LOADING REGISTERS ######" << std::endl;
    for(int i(0); i <= 3; ++i){
        std::string regName = "$a" + std::to_string(i);
        load(dst, regName, regName);
    }
    dst << "###### DONE LOADING REGISTERS ######" << std::endl;
}



void Frame::storeEmptyArray(std::ostream& dst, const std::string& arrayName, const int size, const Type type, bool force){
    Type elementType = addrToType(type);
 
    dst << "###### MAKING SPACE FOR ARRAY ######" << std::endl;
    //Allocate enough space for the array (and for the pointer to the array)
    while(freeWords < size + 1) addWords(dst, scopeMap.back().size());
    
    if(force){
        if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
            scopeMap.back().erase(arrayName);
            scopeMap.back().insert({arrayName, nextFreeAddr - 4 * (freeWords - 1)});
            typeMap.back().erase(arrayName);
            typeMap.back().insert({arrayName, type});

        }else{
            scopeMap.back().insert({arrayName, nextFreeAddr - 4 * (freeWords - 1)});
            typeMap.back().insert({arrayName, type});
        }
        
        nextFreeAddr -= 4;
        freeWords--;
            
    }else{
        if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
            
            //Do nothing (everything is do further down)
            
        }else{assert(0);}    
    }
    
    dst << "###### CALCULATING ADDRESS OF ARRAY ######" << std::endl;
    //The last element in the stack is going to be the pointer to the array
    dst << "addi $t0, $sp, 4" << std::endl;
    dst << "sw $t0, 0($sp)" << std::endl;
    
    dst << "###### STORING ARRAY ELEMENTS ######" << std::endl;
    //Store the elements of the array
    int byteIndex = 0;
    for(int i(0); i < size; ++i){
        if(elementType == Type::INT || elementType == Type::FLOAT){
            
            dst << "sw $zero, " << byteIndex + 4 << "($sp)" << std::endl;
            byteIndex += 4;
        
        //Special case have to store as bytes
        }else if(elementType == Type::CHAR){
            
            dst << "sb $zero, " << byteIndex + 4 << "($sp)" << std::endl;
            ++byteIndex;
        
        }else{assert(0);}
        
    }
    dst << "###### FINISHED STORING ARRAY ELEMENTS ######" << std::endl;
    
    //Reset the stack
    nextFreeAddr -= ((freeWords - 1) * 4) + 4;
    freeWords = 0;
        
}

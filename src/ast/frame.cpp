#include "ast_real/compiler/frame.hpp"
#include "cassert"

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
    dst << "move $fp, $sp\n";
}

void Frame::load(std::ostream& dst, const std::string reg, const std::string varName)const{
    try{
        if(reg == "$f0"){
            dst << "lwc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n"; 
        }else{
            dst << "lw " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
        } 
    }catch(const std::out_of_range& e){
        try{
            argTranslator->load(dst, reg, varName);
        }catch(const std::out_of_range& e){
            if(g_mips_var.find(varName) != g_mips_var.end()){
                dst << "lui " << reg << ",%hi(" << varName << ")" << std::endl;
                dst << "lw " << reg <<  ",%lo(" << varName << ")(" << reg << ")" << std::endl;
                
            }else{
                throw(std::runtime_error("Couldn't find " + varName + " in Frame::load"));
            }
        }      
    }
}

void Frame::store(std::ostream& dst, const std::string reg, const std::string varName, bool force, Type type){
    //This is a tricky case needs care
    if(force){
        if(scopeMap.back().find(varName) != scopeMap.back().end()){
            scopeMap.back().erase(varName);
            scopeMap.back().insert({varName, nextFreeAddr});
            typeMap.back().erase(varName);
            typeMap.back().insert({varName, type});

        }else{
            if(freeWords == 0) addWords(dst, scopeMap.back().size());
            scopeMap.back().insert({varName, nextFreeAddr});
            typeMap.back().insert({varName, type});
            nextFreeAddr -= 4;
            freeWords--;
        }
            
        //This code will be fixed    
        if(reg[1] == 't' || reg[1] == 'v' || reg[1] == '2' || reg[1] == '3')
            dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
            
        else if(reg[1] == 'f')
            dst << "swc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
          
        else
            assert(0);
 
    }else{
        if(scopeMap.back().find(varName) != scopeMap.back().end()){
            //This code will be fixed    
            if(reg[1] == 't' || reg[1] == 'v' || reg[1] == '2' || reg[1] == '3')
                dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
                
            else if(reg[1] == 'f')
                dst << "swc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
              
            else
                assert(0);
            
        }else if(g_mips_var.find(varName) != g_mips_var.end()){
            dst << "lui $t7, %hi(" << varName << ")" << std::endl;
            dst << "sw " << reg << ",%lo(" << varName << ")($t7)" << std::endl;
            
        }else{
            scopeMap.back().insert({varName, nextFreeAddr});
            typeMap.back().insert({varName, type});
            nextFreeAddr -= 4;
            freeWords--;
            
            //This code will be fixed    
            if(reg[1] == 't' || reg[1] == 'v' || reg[1] == '2' || reg[1] == '3')
                dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
                
            else if(reg[1] == 'f')
                dst << "swc1 " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
              
            else
                assert(0);
           
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

void Frame::saveArguments(std::ostream& dst, const std::vector<const Expr*>* argumentExprList){
    //Looks compilcated but we need to do it this way so that the $sp doesn't change
    if(argumentExprList != NULL){
        //Vector to hold the names of the Variables of the expressions
        std::vector<std::string> argumentNames;
        for(int i(0); i < argumentExprList->size(); ++i){
            std::string exprName = makeName();
            argumentNames.push_back(exprName);
            const Expr* expr = static_cast<const Expr*>(argumentExprList->at(i));
            expr->printMipsE(dst, argumentNames.back(), this);
        }
        
        //At this point all the expressions have been evaluated and are saved in the stack
        dst << "addiu $sp, $sp, -" << 4*argumentExprList->size() << std::endl;
        //From this point $sp can't change!!!
        for(int i(0); i < argumentNames.size(); ++i){
            if(i < 4) load(dst, std::string("$a").append(std::to_string(i)), argumentNames.at(i));
            else{
                //Store the arguments in the correct position of the frame;
                load(dst, std::string("$t0"), argumentNames.at(i));
                dst << "sw $t0, " << 4*(i) << "($sp)" << std::endl;
            }
        }
    }
}


void Frame::storeArray(std::ostream& dst, const std::string& arrayName, const std::vector<const Expr*>* argumentExprList, bool force){
    if(argumentExprList != NULL){
        //Allocate enough space for the array
        while(freeWords < argumentExprList->size()) addWords(dst, scopeMap.back().size());
        bool ok = scopeMap.back().insert({arrayName, nextFreeAddr}).second;
        if(ok){
            //No shadowing
            scopeMap.back().insert({arrayName, nextFreeAddr});
            for(int i(0); i < argumentExprList->size(); ++i){
                dst << "li $t0, " << argumentExprList->at(i)->getId() << std::endl;
                dst << "sw $t0, " << scopeMap.back().at(arrayName) - 4*i << "($fp)" << std::endl;
                nextFreeAddr -= 4;
                freeWords--;
            }
        }else{
            //Shadowing
            for(int i(0); i < argumentExprList->size(); ++i){
                scopeMap.back().erase(arrayName);
                scopeMap.back().insert({arrayName, nextFreeAddr});
                dst << "li $t0, " << argumentExprList->at(i)->getId() << std::endl;
                dst << "sw $t0, " << scopeMap.back().at(arrayName) - 4*i << "($fp)" << std::endl;
                nextFreeAddr -= 4;
                freeWords--;
            }
                
        }
    }
}

void Frame::loadArrayElement(std::ostream& dst, const std::string& reg, const std::string& arrayName, const Expr* index){
    dst << "######## Loading Array Element ########" << std::endl;
    //Calculate the index
    std::string indexName = makeName();
    index->printMipsE(dst, indexName, this);
    load(dst, "$t0", indexName);
    //Multiply by 4
    dst << "sll $t0, $t0, 2" << std::endl;
    dst << "subu $t1, $fp, $t0" << std::endl;
    try{
        dst << "lw " << reg << ", " << scopeMap.back().at(arrayName) << "($t1)" << std::endl;
    }catch(const std::out_of_range& e){
            dst << "lui $t1, %hi(" << arrayName << ")" << std::endl;
            dst << "addiu $t1, $t1, %lo(" << arrayName << ")" << std::endl;
            dst << "addu $t1, $t1, $t0" << std::endl;
            dst << "lw " << reg << ", " << "0($t1)" << std::endl;
    }
    
    dst << "######## Done Loading ########" << std::endl;
}

void Frame::storeArrayElement(std::ostream& dst, const std::string& reg, const PostfixExpr* postfixExpr){
    dst << "######## Storing Array Element ########" << std::endl;
    
    //We have to store so that we don't overwright it
    std::string safetyStore(makeName());
    store(dst, reg, safetyStore);
    
    //Get the identifier of the array
    std::string arrayName = postfixExpr->getId();
    
    //Evaluate the index of the array element
    std::string indexName = makeName();
    postfixExpr->evaluateArgument(dst, indexName, this);
    load(dst, "$t0", indexName);
    dst << "sll $t0, $t0, 2" << std::endl;
    
    //Check if the array is in local scope
    if(scopeMap.back().find(arrayName) != scopeMap.back().end()){
        dst << "subu $t0, $fp, $t0" << std::endl;
        load(dst, "$t1", safetyStore);
        dst << "sw $t1, " << scopeMap.back().at(arrayName) << "($t0)" << std::endl;
    }else{
        //It must be in Global scope
        
        //Find the address of the array
        dst << "lui $t1, %hi(" << arrayName << ")" << std::endl;
        dst << "addiu $t1, $t1, %lo(" << arrayName << ")" << std::endl;
        
        //Find the address of the element
        dst << "addu $t1, $t1, $t0" << std::endl;
                
        //Store
        load(dst, "$t2", safetyStore);
        dst << "sw $t2, 0($t1)" << std::endl;
    }
    
    dst << "######## Done Storing ########" << std::endl;
}

void Frame::loadAddr(std::ostream& dst, const std::string& reg, const std::string& varName)const{
    //Check if the variable is in local scope
    if(scopeMap.back().find(varName) != scopeMap.back().end()){
        dst << "addi " << reg << ", $fp, " << scopeMap.back().at(varName) << std::endl;
    }else{
        //Assuming that it is in Global
        
        //Find the address of the variable
        dst << "lui " << reg << ", %hi(" << varName << ")" << std::endl;
        dst << "addiu " << reg << ", " << reg << ", %lo(" << varName << ")" << std::endl;
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

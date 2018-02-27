#include "ast_real/compiler/frame.hpp"

void Frame::addWords(std::ostream& dst, int n){
    int bytes = n*4;
    dst << "addiu $sp, $sp, -" << bytes << "\n";
    freeWords += n;
}

Frame::Frame(std::ostream& dst, const DirectDeclarator* directDeclarator)
    :argTranslator(directDeclarator){

    std::unordered_map<std::string, int> temp;
    scopeMap.push_back(temp);
    
    //Allocate the minumum frame
    scopeMap.front().insert({"oldFramePointer", 8});
    scopeMap.front().insert({"returnAddr", 4});
    
    freeWords = 0;
    nextFreeAddr = 0;
    
    dst << "addiu $sp, $sp, -12\n";
    dst << "sw $fp, 8($sp)\n";
    dst << "sw $31, 4($sp)\n";
    dst << "move $fp, $sp\n";
}

void Frame::load(std::ostream& dst, const std::string reg, const std::string varName)const{
    try{
        dst << "lw " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
    }catch(const std::out_of_range& e){
        argTranslator.load(dst, reg, varName);
    }
}

void Frame::store(std::ostream& dst, const std::string reg, const std::string varName, bool force){
    if(freeWords == 0) addWords(dst, scopeMap.back().size());
    bool ok = scopeMap.back().insert({varName, nextFreeAddr}).second;
    if(ok){
        nextFreeAddr -= 4;
        freeWords--;
    }else{
        //We want to replace the value in the stack and not create a new one
        if(force){
            scopeMap.back().erase(varName);
            scopeMap.back().insert({varName, nextFreeAddr});
            nextFreeAddr -= 4;
            freeWords--;
        }
    }    
        dst << "sw " << reg << ", " << scopeMap.back().at(varName) << "($fp)\n";
}

void Frame::newScope(){
    std::unordered_map<std::string, int> temp = scopeMap.back();
    scopeMap.push_back(temp);
}

void Frame::deleteScope(){
    scopeMap.pop_back();
}
   
void Frame::clean(std::ostream& dst)const{
    dst << "move $sp, $fp\n";
    load(dst, "$31", "returnAddr");
    load(dst, "$fp", "oldFramePointer");
    dst << "addiu $sp, $sp, +12\n";
    dst << "jr $31\n";
    dst << "nop\n";
}

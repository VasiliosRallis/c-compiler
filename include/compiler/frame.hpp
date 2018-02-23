#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <unordered_map>
#include <string>
#include <iostream>

static unsigned gUniqueIndex = 0;
static std::string makeName(const std::string& base = "temp"){
    ++gUniqueIndex;
    return "__" + base + "__" + std::to_string(gUniqueIndex);
}

class Frame{
private:
    int freeWords;
    int nextFreeAddr;
    std::unordered_map<std::string, int> frameMap;
    
public:
    Frame(std::ostream& dst){
        //Allocate the minumum frame
       
        frameMap.insert({"oldFramePointer", 8});
        frameMap.insert({"returnAddr", 4});
        
        freeWords = 0;
        nextFreeAddr = 0;
        
        dst << "addiu $sp, $sp, -12\n";
        dst << "sw $fp, 8($sp)\n";
        dst << "sw $31, 4($sp)\n";
        dst << "move $fp, $sp\n";
    }
    
    void addWords(std::ostream& dst, int n){
        int bytes = n*4;
        dst << "addiu $sp, $sp, -" << bytes << "\n";
        freeWords += n;
    }   
    
    void load(std::ostream& dst, std::string reg, std::string varName)const{
        dst << "lw " << reg << ", " << frameMap.at(varName) << "($fp)\n";
    }
    
    void store(std::ostream& dst, std::string reg, std::string varName){
        if(freeWords == 0) addWords(dst, 1);
        
        frameMap.insert({varName, nextFreeAddr});
        nextFreeAddr -= 4;
        freeWords--;
        
        dst << "sw " << reg << ", " << frameMap.at(varName) << "($fp)\n";
    }
       
    void clean(std::ostream& dst)const{
        dst << "move $sp, $fp\n";
        load(dst, "$31", "returnAddr");
        load(dst, "$fp", "oldFramePointer");
        dst << "addiu $sp, $sp, +12\n";
        dst << "jr $31\n";
        dst << "nop\n";
    }    
};
#endif

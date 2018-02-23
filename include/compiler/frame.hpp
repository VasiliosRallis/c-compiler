#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <unordered_map>
#include <string>
#include <iostream>


class Frame{
private:
    int freeWords;
    int nextFreeAddr;
    std::unordered_map<std::string, int> frameMap;
    
public:
    Frame(std::ostream& dst){
        //Allocate the minumum frame
        
        
        frameMap.insert({"$sp", 8});
        frameMap.insert({"$31", 4});
        
        freeWords = 0;
        nextFreeAddr = 0;
        
        dst << "addiu $sp, $sp, -12\n";
        dst << "sw $fp, 8($sp)\n";
        dst << "sw $31, 4($sp)\n";
        dst << "move $fp, $sp\n";
    }
    
    
    void load(std::ostream& dst, std::string varName)const{
        dst << "lw " << varName << ", " << frameMap.at(varName) << "($fp)\n";
    }
    
    void store(std::ostream& dst, std::string varName){
        frameMap.insert({varName, nextFreeAddr});
        nextFreeAddr -= 4;
        
        dst << "sw " << varName << ", " << frameMap.at(varName) << "($fp)\n";
    }    
        
            
    
    void clean(std::ostream& dst)const{
        dst << "move $sp, $fp\n";
        load(dst, "$31");
        dst << "lw $fp, 8($sp)\n";
        dst << "addiu $sp, $sp, +12\n";
        dst << "jr $31\n";
        dst << "nop\n";
    }    
};
#endif

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


//This class will simulate the stack frame of each function when it's called
class Frame{
private:
    //freeWords will idicate how many un allocated words are available in the frame
    //e.g. If you do addiu $sp, $sp -12 and you allocate $sp+8 you onl have one free frame left ($sp + 4)
    //This is beacause I'm assuming that we cant write to $sp + 0
    int freeWords;
    //nextFreeAddr will indicate the next free address that we can write to
    //This is an address relative to the $fp which remains constant throught the lifetime of the function
    int nextFreeAddr;
    
    //This unordered man holds the identifier name which can be the actual name in the c program (e.g. int x; we would
    //store an indetifier a. The int is that address of the frame relative to the $fp
    std::unordered_map<std::string, int> frameMap;
    
    
    void addWords(std::ostream& dst, int n){
        int bytes = n*4;
        dst << "addiu $sp, $sp, -" << bytes << "\n";
        freeWords += n;
    }  
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
    
    void load(std::ostream& dst, const std::string reg, const std::string varName)const{
        dst << "lw " << reg << ", " << frameMap.at(varName) << "($fp)\n";
    }
    
    void store(std::ostream& dst, const std::string reg, const std::string varName){
        if(freeWords == 0) addWords(dst, frameMap.size());
        bool ok = frameMap.insert({varName, nextFreeAddr}).second;
        if(!ok){
            frameMap.erase(varName);
            frameMap.insert({varName, nextFreeAddr});
        }
            
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

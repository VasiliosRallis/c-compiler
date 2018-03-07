#ifndef FRAME_HPP_
#define FRAME_HPP_

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

class Frame;

#include "ast_real/compiler/argTranslator.hpp"
#include "ast_real/ast/directDeclarator.hpp"
#include "ast_real/ast/node.hpp"
#include "ast_real/ast/expr.hpp"
#include "ast_real/ast/postfixExpr.hpp"

extern std::vector<std::string> g_mips_var;

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
    std::vector<std::unordered_map<std::string, int> > scopeMap;
    
    void addWords(std::ostream& dst, int n);
    
    ArgTranslator* const argTranslator;
    
public:
    Frame(std::ostream& dst, const DirectDeclarator* directDeclarator);
    
    void load(std::ostream& dst, const std::string reg, const std::string varName)const;
    
    void store(std::ostream& dst, const std::string reg, const std::string varName, bool force = false);
    
    void newScope();
    
    void deleteScope();
       
    void clean(std::ostream& dst)const;
    
    void saveArguments(std::ostream& dst, const std::vector<const Expr*>* argExprList);
    
    void storeArray(std::ostream& dst, const std::string& arrayName, const std::vector<const Expr*>* argumentExprList, bool force = false);
    
    void loadArrayElement(std::ostream& dst, const std::string& reg, const std::string& arrayName, const Expr* index);
    
    void storeArrayElement(std::ostream& dst, const std::string& reg, const PostfixExpr* postfixExpr);
    
    void loadAddr(std::ostream& dst, const std::string& reg, const std::string& varName)const;
};

static unsigned gUniqueIndex = 0;
static std::string makeName(const std::string& base = "temp"){
    ++gUniqueIndex;
    return "__" + base + "__" + std::to_string(gUniqueIndex);
}

#endif

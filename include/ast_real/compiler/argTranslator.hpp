#ifndef ARG_TRANSLATOR_HPP
#define ARG_TRANSLATOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "ast_real/ast/directDeclarator.hpp"

class ArgTranslator{
private:
    std::unordered_map<std::string, std::string> argMap;
  
public:
    ArgTranslator(const DirectDeclarator* directDeclarator);
    
    void load(std::ostream& dst, const std::string& destName, const std::string& varName)const;
};

#endif

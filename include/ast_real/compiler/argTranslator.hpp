#ifndef ARG_TRANSLATOR_HPP
#define ARG_TRANSLATOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

class ArgTranslator;

#include "ast_real/ast/directDeclarator.hpp"
#include "ast_real/ast/parameterDeclaration.hpp"

class ArgTranslator{
private:
    std::unordered_map<std::string, std::string> argMap;
    std::unordered_map<std::string, Type> argTypeMap;
  
public:
    ArgTranslator(const DirectDeclarator* directDeclarator);
    
    void load(std::ostream& dst, const std::string& destName, const std::string& varName)const;
    
    Type loadType(const std::string& id)const;
};

#endif

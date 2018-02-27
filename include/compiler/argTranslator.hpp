#ifndef ARG_TRANSLATOR_HPP
#define ARG_TRANSLATOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

class argTranslator{
private:
    std::unordered_map<std::string, std::string> argMap;
  
public:
    argTranslator(const DirectDeclarator* directDeclarator){
        //const std::vector<const ParameterDeclaration*> = directDeclarator->getParameterList();
    }
};

#endif

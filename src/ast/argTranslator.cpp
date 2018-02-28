#include "ast_real/compiler/argTranslator.hpp"

ArgTranslator::ArgTranslator(const DirectDeclarator* directDeclarator){
    const std::vector<const ParameterDeclaration*>* v1 = directDeclarator->getParameterList();
    if(v1 != NULL){
        for(int i(0); i < v1->size(); ++i){
            argMap.insert({v1->at(i)->getId(), std::string("$a").append(std::to_string(i))});
            //TODO: Think about other problems
        }
    }
}


void ArgTranslator::load(std::ostream& dst, const std::string& destName, const std::string& varName)const{
    dst << "move " << destName << ", " << argMap.at(varName) << std::endl;
}


#include "ast_real/compiler/argTranslator.hpp"

ArgTranslator::ArgTranslator(const DirectDeclarator* directDeclarator){
    const std::vector<const ParameterDeclaration*>* v1 = directDeclarator->getParameterList();
    if(v1 != NULL){
        for(int i(0); i < v1->size(); ++i){
            argTypeMap.insert({v1->at(i)->getId(), v1->at(i)->getType()});
            
            if(i < 4) argMap.insert({v1->at(i)->getId(), std::string("$a").append(std::to_string(i))});
            //Store the address of the argument in the statck relative to the previous $sp
            else argMap.insert({v1->at(i)->getId(), std::to_string(4 * i + 12)});
        }
    }
}


void ArgTranslator::load(std::ostream& dst, const std::string& destName, const std::string& varName)const{
    if(argMap.at(varName).at(0) == '$') dst << "move " << destName << ", " << argMap.at(varName) << std::endl;
    else{
        //Load the previous $sp
        dst << "lw " << destName << ", " << std::stoi(argMap.at(varName)) << "($fp)" << std::endl;
    }
}

Type ArgTranslator::loadType(const std::string& id)const{
    try{
        return argTypeMap.at(id);
    
    }catch(const std::out_of_range& e){
        throw std::runtime_error("Tried to load type of uknown id!");
    }
}

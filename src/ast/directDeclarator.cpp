#include <vector>
#include <string>
#include <unordered_map>
#include <cassert>

#include "ast_real/ast/directDeclarator.hpp"

class ParameterDeclaration;
class Expr;

extern std::unordered_map<std::string, std::vector<Type> > function_decl;
extern std::unordered_map<std::string, std::vector<Type> > function_type;

DirectDeclarator::DirectDeclarator(StrPtr _id, StrPtr _s1, const std::vector<const ParameterDeclaration*>* _v1, StrPtr _s2, const Expr* _expr)
    :StringNode(_id), s1(_s1), v1(_v1), s2(_s2), expr(_expr){}
    
void DirectDeclarator::print(std::ostream& dst)const{
    dst << *id;
    if(s1 != NULL) dst << *s1;
    if(v1 != NULL){
        for(int i(0); i < v1->size(); ++i){
            v1->at(i)->printPy(dst);
            if(i < v1->size() - 1) dst << ",";
        }
    }
    if(s2 != NULL) dst << *s2;
}

void DirectDeclarator::printPy(std::ostream& dst, int depth)const{
    for(int i(0); i < depth; ++i) dst << "\t";
    print(dst);
}

void DirectDeclarator::printMips(std::ostream& dst, Frame* framePtr, Type type)const{
    //Set the default value for an unitialized variable to zero
    if(s1 != NULL){
        if(*s1 == "["){
            int arraySize = expr->eval();
            framePtr->storeEmptyArray(dst, *id, arraySize, type, true);       
        }
        
    }else{framePtr->store(dst, "$zero", *id, type, true);}
}

std::string DirectDeclarator::getId()const{
    return std::string(*id);
}

const std::vector<const ParameterDeclaration*>* DirectDeclarator::getParameterList()const{
    return v1;
}

void DirectDeclarator::printGMips(std::ostream& dst, Type type)const{
    if(s1 == NULL){
        if(type == Type::CHAR){
            dst << "\t.comm\t" << *id << ",1,1" << std::endl;
        
        }else if(type == Type::INT || type == Type::FLOAT){
            dst << "\t.comm\t" << *id << ",4,4" << std::endl;
            
        }else if(type == Type::DOUBLE){
            dst << "\t.comm\t" << *id << ",8,8" << std::endl;
            
        }else if(isAddr(type)){
            assert(0); //Should never happen
        }else{
            std::cerr << "Type: " << (int)type << std::endl;
            assert(0);
        }
        
        g_mips_var.insert({*id, type});
        
    }else{
        //Check if it is a function declaration
        if(*s1 == "("){
            std::vector<Type> v;
            //Pushback the return type
            v.push_back(type);
            
            if(v1 != NULL){
                for(auto i = v1->begin(); i != v1->end(); ++i){
                    v.push_back((*i)->getType(NULL));
                }
            }
            
            function_decl.insert({*id, v});
        
        }else if(*s1 == "["){
            //Here we are storing char in a 4 bytes (this isn't how gcc does it)
            if(type == Type::CHAR_ADDR || type == Type::INT_ADDR || type == Type::FLOAT_ADDR){
                dst << "\t.comm\t" << *id << "," << 4*expr->eval() << ",4" << std::endl;
                
            }else if(type == Type::DOUBLE_ADDR){
                dst << "\t.comm\t" << *id << "," << 8*expr->eval() << ",8" << std::endl;
                
            }else{assert(0);}
            
            g_mips_var.insert({*id, type});
            
        }else if(*s1 == "*"){
            //It's not an array (i.e. simple pointer)
            dst << "\t.comm\t" << *id << ",4,4" << std::endl;
            
            g_mips_var.insert({*id, type});
        }  
    }
}

bool DirectDeclarator::isPointer()const{
    if(s1 != NULL){
        //Think of arrays at pointers
        if(*s1 == "*" || *s1 == "[") return true;
        else return false;
    }else{
        return false;
    }
}

void DirectDeclarator::insertFTypes(const Type returnType)const{
    if(s1 != NULL){
        if(*s1 == "("){
            std::vector<Type> v;
            v.push_back(returnType);

            if(v1 != NULL){
                for(auto i = v1->begin(); i != v1->end(); ++i){
                    v.push_back((*i)->getType(NULL));
                }
            }
            
            function_type.insert({*id, v});  
        }else{assert(0);} //Should never happen
        
    }else{assert(0);} //Should never happen
}

#ifndef TYPE_CONV_HPP
#define TYPE_CONV_HPP

#include <iostream>
#include <string>

#include "ast_real/ast/node.hpp"

class TypeConv{
public:
    static void convert(std::ostream& dst, const Type outT, const Type inT, const std::string& outReg, const std::string& inReg);
    
private:
    static void cIntFloat(std::ostream& dst, const std::string& outReg, const std::string& inReg);
    static void cFloatInt(std::ostream& dst, const std::string& outReg, const std::string& inReg);   
    //static void cIntDouble(std::ostream& dst, const std::string& outReg, const std::string& inReg);
};

#endif

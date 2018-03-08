#include "ast_real/compiler/typeConv.hpp"

void TypeConv::convert(std::ostream& dst, const Type outT, const Type inT, const std::string& outReg, const std::string& inReg){
    if(outT == Type::INT && inT == Type::FLOAT){
        cIntFloat(dst, outReg, inReg);
        
    }else{
        throw std::runtime_error("Haven't implemented conversion for those types");
    }
}

void TypeConv::cIntFloat(std::ostream& dst, const std::string& outReg, const std::string& inReg){
    dst << ".set macro" << std::endl;
    dst << "trunc.w.s " << inReg << ", " << inReg << ", $28" << std::endl;
    dst << ".set nomacro" << std::endl;
    dst << "mfc1 " << outReg << ", " << inReg << std::endl;
}

#ifndef histogram_hpp
#define histogram_hpp

#include <string>


enum TokenType{
    NONE,
    NUMBER,
    OPEN_CUR_BRACKET,
    CLOSE_CUR_BRACKET,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHERSIS,
    SEMI_COLON,
    RETURN,
    C_INT,
    IDENTIFIER
};

union TokenValue{
    double numberValue;
    std::string* wordValue;
};

// This is a global variable used to move the
// attribute value from the lexer back to the
// main program.
// By convention it is called yylval, as that is
// the name that will be automatially generated
// by Bison (see next lab).
extern TokenValue yylval;

// This is the lexer function defined by flex. Each
// time it is called, a token type value will be
// returned.
extern int yylex();

#endif

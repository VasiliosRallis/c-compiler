%option noyywrap

%{

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "lexer.hpp"

// This is to work around an irritating bug in Flex
// https://stackoverflow.com/questions/46213840/get-rid-of-warning-implicit-declaration-of-function-fileno-in-flex
extern "C" int fileno(FILE *stream);

TokenType reportToken(TokenType tokenType, const char* tokenValue);

std::vector<std::string> translator{

    "NONE",
    "NUMBER",
    "OPEN_CUR_BRACKET",
    "CLOSE_CUR_BRACKET",
    "OPEN_PARENTHESIS",
    "CLOSE_PARENTHERSIS",
    "SEMI_COLON",
    "RETURN",
    "C_INT",
    "IDENTIFIER"
};

/* End the embedded code section. */
%}

IDENTIFIER              [A-Za-z_][A-Za-z_0-9]*
INTEGER			[0-9]+
%%

int    			{ return reportToken(C_INT, yytext); }
return 			{ return reportToken(RETURN, yytext); }

\{			{ return reportToken(OPEN_CUR_BRACKET, yytext); }
\}			{ return reportToken(CLOSE_CUR_BRACKET, yytext); }
\(			{ return reportToken(OPEN_PARENTHESIS, yytext); }
\)			{ return reportToken(CLOSE_PARENTHERSIS, yytext); }
\;			{ return reportToken(SEMI_COLON, yytext); }

{INTEGER}		{ return reportToken(NUMBER, yytext); }
{IDENTIFIER}		{ return reportToken(IDENTIFIER, yytext); }

.|[\n] 			{ }


%%

TokenType reportToken(TokenType tokenType, const char* tokenValue){
	//Use cerr for loging/debigging purposes 
	std::cerr << "Token: " << std::setw(20) << translator[tokenType] << std::setw(20) << std::string(tokenValue) << std::endl;
	
	return tokenType;;
}

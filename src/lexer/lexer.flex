%option noyywrap

%{

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "/home/kayne/Desktop/ctom/src/parser/parser.tab.hpp"

// This is to work around an irritating bug in Flex
// https://stackoverflow.com/questions/46213840/get-rid-of-warning-implicit-declaration-of-function-fileno-in-flex
extern "C" int fileno(FILE *stream);

yytokentype reportToken(yytokentype tokenType, const char* tokenValue);

std::vector<std::string> translator{

    "NONE",
    "T_INT",
    "T_RETURN",
    "T_LBRACKET",
    "T_RBRACKET ",
    "T_LCURLBRACKET" ,
    "T_RCURLBRACKET",
    "T_SEMICOLON" ,
    "T_IDENTIFIER ",
    "T_INT_CONSTANT" 
};

/* End the embedded code section. */
%}

IDENTIFIER              [A-Za-z_][A-Za-z_0-9]*
INTEGER			[0-9]+
%%

int    			{ yylval.string = new std::string(yytext); return reportToken(T_INT, yytext); }
return 			{ return reportToken(T_RETURN, yytext); }

\{			{ return reportToken(T_LCURLBRACKET, yytext); }
\}			{ return reportToken(T_RCURLBRACKET, yytext); }
\(			{ return reportToken(T_LBRACKET, yytext); }
\)			{ return reportToken(T_RBRACKET, yytext); }
\;			{ yylval.string = new std::string(";"); return reportToken(T_SEMICOLON, yytext); }

{INTEGER}		{ return reportToken(T_INT_CONSTANT, yytext); }
{IDENTIFIER}		{ yylval.string = new std::string(yytext); return reportToken(T_IDENTIFIER, yytext); }

.|[\n] 			{ }


%%
void yyerror (char const *s)
{
  fprintf (stderr, "Flex Error: %s\n", s); /* s is the text that wasn't matched */
  exit(1);
}

yytokentype reportToken(yytokentype tokenType, const char* tokenValue){
	//Use cerr for loging/debigging purposes 
	std::cerr << "Token: " << std::setw(20) << translator[tokenType-257] << std::setw(20) << std::string(tokenValue) << std::endl;
	
	return tokenType;;
}

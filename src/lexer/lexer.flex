%option noyywrap

%{

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "parser.tab.hpp"

// This is to work around an irritating bug in Flex
// https://stackoverflow.com/questions/46213840/get-rid-of-warning-implicit-declaration-of-function-fileno-in-flex
extern "C" int fileno(FILE *stream);

yytokentype reportToken(yytokentype tokenType, const char* tokenValue);

std::vector<std::string> translator{

    "NONE",
    "T_INT",
    "T_CHAR",
    "T_VOID",
    "T_SHORT",
    "T_LONG" ,
    "T_FLOAT",
    "T_DOUBLE",
    "T_SIGNED" ,
    "T_UNSIGNED",
    "T_RETURN",
    "T_AUTO",
    "T_STRUCT",
    "T_BREAK",
    "T_ELSE",
    "T_SWITCH",
    "T_CASE",
    "T_ENUM",
    "T_REGISTER",
    "T_TYPEDEF",
    "T_EXTERN",
    "T_UNION",
    "T_CONST",
    "T_CONTINUE",
    "T_FOR",
    "T_DEFAULT",
    "T_GOTO",
    "T_SIZEOF",
    "T_VOLATILE",
    "T_DO",
    "T_IF",
    "T_STATIC",
    "T_WHILE",
    "T_EQUAL",
    "T_RIGHT_S_ASSIGN",
    "T_LEFT_S_ASSIGN",
    "T_ADD_ASSIGN",
    "T_SUB_ASSIGN",
    "T_MUL_ASSIGN",
    "T_DIV_ASSIGN",
    "T_MOD_ASSIGN",
    "T_AND_ASSIGN",
    "T_XOR_ASSIGN",
    "T_OR_ASSIGN",
    "T_LBRACKET",
    "T_RBRACKET",
    "T_LCURLBRACKET",
    "T_RCURLBRACKET",
    "T_SEMICOLON",
    "T_COMMA",
    "T_QUESTION",
    "T_COLON",
    "T_IDENTIFIER",
    "T_INT_CONSTANT",
    "T_STR_LIT",
    "T_AMPERSAND",
    "T_EXCLAMATION",
    "T_TILDE",
    "T_INCREMENT",
    "T_DECREMENT",
    "T_LOGICAL_OR",
    "T_LOGICAL_AND",
    "T_OR",
    "T_AND",
    "T_XOR",
    "T_EQUALITY",
    "T_INEQUALITY",
    "T_SMALLER",
    "T_GREATER",
    "T_GREATER_EQUAL",
    "T_SMALLER_EQUAL",
    "T_SHIFT_L",
    "T_SHIFT_R",
    "T_PLUS",
    "T_MINUS",
    "T_DIV",
    "T_MULT",
    "T_MOD",
    "T_ARROW",
    "T_DOT",
    "T_SQUARE_LBRACKET",
    "T_SQUARE_RBRACKET"
};
 
/* End the embedded code section. */
%}

IDENTIFIER      [A-Za-z_][A-Za-z_0-9]*
INTEGER			[0-9]+

S_CHAR          [^\\"\n]
CHAR_SEQ        {CHAR}+
STR_LIT         "{CHAR_SEQ}"|""

%%

int    		    {yylval.string = new std::string(yytext); return reportToken(T_INT, yytext);}
char            {yylval.string = new std::string(yytext); return reportToken(T_CHAR, yytext);}
void            {yylval.string = new std::string(yytext); return reportToken(T_VOID, yytext);}
short           {yylval.string = new std::string(yytext); return reportToken(T_SHORT, yytext);}
long            {yylval.string = new std::string(yytext); return reportToken(T_LONG, yytext);}
float           {yylval.string = new std::string(yytext); return reportToken(T_FLOAT, yytext);}
double          {yylval.string = new std::string(yytext); return reportToken(T_DOUBLE, yytext);}
signed          {yylval.string = new std::string(yytext); return reportToken(T_SIGNED, yytext);}
unsigned        {yylval.string = new std::string(yytext); return reportToken(T_UNSIGNED, yytext);}

auto            {yylval.string = new std::string(yytext); return reportToken(T_AUTO, yytext);}
void            {yylval.string = new std::string(yytext); return reportToken(T_STRUCT, yytext);}
break           {yylval.string = new std::string(yytext); return reportToken(T_BREAK, yytext);}
else            {yylval.string = new std::string(yytext); return reportToken(T_ELSE, yytext);}
switch          {yylval.string = new std::string(yytext); return reportToken(T_SWITCH, yytext);}
case            {yylval.string = new std::string(yytext); return reportToken(T_CASE, yytext);}
enum            {yylval.string = new std::string(yytext); return reportToken(T_ENUM, yytext);}
register        {yylval.string = new std::string(yytext); return reportToken(T_REGISTER, yytext);}
typedef         {yylval.string = new std::string(yytext); return reportToken(T_TYPEDEF, yytext);}
extern          {yylval.string = new std::string(yytext); return reportToken(T_EXTERN, yytext);}
union           {yylval.string = new std::string(yytext); return reportToken(T_UNION, yytext);}
const           {yylval.string = new std::string(yytext); return reportToken(T_CONST, yytext);}
continue        {yylval.string = new std::string(yytext); return reportToken(T_CONTINUE, yytext);}
for             {yylval.string = new std::string(yytext); return reportToken(T_FOR, yytext);}
default         {yylval.string = new std::string(yytext); return reportToken(T_DEFAULT, yytext);}
goto            {yylval.string = new std::string(yytext); return reportToken(T_GOTO, yytext);}
sizeof          {yylval.string = new std::string(yytext); return reportToken(T_SIZEOF, yytext);}
volatile        {yylval.string = new std::string(yytext); return reportToken(T_VOLATILE, yytext);}
do              {yylval.string = new std::string(yytext); return reportToken(T_DO, yytext);}
if              {yylval.string = new std::string(yytext); return reportToken(T_IF, yytext);}
static          {yylval.string = new std::string(yytext); return reportToken(T_STATIC, yytext);}
while           {yylval.string = new std::string(yytext); return reportToken(T_WHILE, yytext);}
return 		    {yylval.string = new std::string(yytext); return reportToken(T_RETURN, yytext);}

\=		        {yylval.string = new std::string(yytext); return reportToken(T_EQUAL, yytext);}
\>\>=           {yylval.string = new std::string(yytext); return reportToken(T_RIGHT_S_ASSIGN, yytext);}
\<\<=           {yylval.string = new std::string(yytext); return reportToken(T_LEFT_S_ASSIGN, yytext);}
\+=             {yylval.string = new std::string(yytext); return reportToken(T_ADD_ASSIGN, yytext);}
\-=             {yylval.string = new std::string(yytext); return reportToken(T_SUB_ASSIGN, yytext);}
\*=             {yylval.string = new std::string(yytext); return reportToken(T_MUL_ASSIGN, yytext);}
\/=             {yylval.string = new std::string(yytext); return reportToken(T_DIV_ASSIGN, yytext);}
\%=             {yylval.string = new std::string(yytext); return reportToken(T_MOD_ASSIGN, yytext);}
&=              {yylval.string = new std::string(yytext); return reportToken(T_AND_ASSIGN, yytext);}
\^=             {yylval.string = new std::string(yytext); return reportToken(T_XOR_ASSIGN, yytext);}
\|=             {yylval.string = new std::string(yytext); return reportToken(T_OR_ASSIGN, yytext);}

\+\+            {yylval.string = new std::string(yytext); return reportToken(T_INCREMENT, yytext);}
\-\-            {yylval.string = new std::string(yytext); return reportToken(T_DECREMENT, yytext);}

\!              {yylval.string = new std::string(yytext); return reportToken(T_EXCLAMATION, yytext);}
\?              {yylval.string = new std::string(yytext); return reportToken(T_QUESTION, yytext);}
\:              {yylval.string = new std::string(yytext); return reportToken(T_COLON, yytext);}

\|\|            {yylval.string = new std::string(yytext); return reportToken(T_LOGICAL_OR, yytext);}
\&\&            {yylval.string = new std::string(yytext); return reportToken(T_LOGICAL_AND, yytext);}

\|              {yylval.string = new std::string(yytext); return reportToken(T_OR, yytext);}
\^              {yylval.string = new std::string(yytext); return reportToken(T_XOR, yytext);}
\&              {yylval.string = new std::string(yytext); return reportToken(T_AND, yytext);}

\=\=            {yylval.string = new std::string(yytext); return reportToken(T_EQUALITY, yytext);}
\!\=            {yylval.string = new std::string(yytext); return reportToken(T_INEQUALITY, yytext);}

\<              {yylval.string = new std::string(yytext); return reportToken(T_SMALLER, yytext);}
\>              {yylval.string = new std::string(yytext); return reportToken(T_GREATER, yytext);}
\<\=            {yylval.string = new std::string(yytext); return reportToken(T_SMALLER_EQUAL, yytext);}
\>\=            {yylval.string = new std::string(yytext); return reportToken(T_GREATER_EQUAL, yytext);}

\<\<            {yylval.string = new std::string(yytext); return reportToken(T_SHIFT_L, yytext);}
\>\>            {yylval.string = new std::string(yytext); return reportToken(T_SHIFT_R, yytext);}

\+              {yylval.string = new std::string(yytext); return reportToken(T_PLUS, yytext);}
\-              {yylval.string = new std::string(yytext); return reportToken(T_MINUS, yytext);}

\*              {yylval.string = new std::string(yytext); return reportToken(T_MULT, yytext);}
\/              {yylval.string = new std::string(yytext); return reportToken(T_DIV, yytext);}
\%              {yylval.string = new std::string(yytext); return reportToken(T_MOD, yytext);}

\-\>            {yylval.string = new std::string(yytext); return reportToken(T_ARROW, yytext);}
\.              {yylval.string = new std::string(yytext); return reportToken(T_DOT, yytext);}
\[              {yylval.string = new std::string(yytext); return reportToken(T_SQUARE_LBRACKET, yytext);}
\]              {yylval.string = new std::string(yytext); return reportToken(T_SQUARE_RBRACKET  , yytext);}

\{			    {yylval.string = new std::string(yytext); return reportToken(T_LCURLBRACKET, yytext);}
\}			    {yylval.string = new std::string(yytext); return reportToken(T_RCURLBRACKET, yytext); }
\(			    {yylval.string = new std::string(yytext); return reportToken(T_LBRACKET, yytext); }
\)			    {yylval.string = new std::string(yytext); return reportToken(T_RBRACKET, yytext); }
\;			    {yylval.string = new std::string(yytext); return reportToken(T_SEMICOLON, yytext); }
\,              {yylval.string = new std::string(yytext); return reportToken(T_COMMA, yytext);}

{INTEGER}		{yylval.string = new std::string(yytext); return reportToken(T_INT_CONSTANT, yytext);}
{IDENTIFIER}	{yylval.string = new std::string(yytext); return reportToken(T_IDENTIFIER, yytext);}
{STR_LIT}       {yylval.string = new std::string(yytext); return reportToken(T_STR_LIT, yytext);}

#.*             {  } //remove preprocessed information
\/\/.*            {   } // single line comments
\/\*([^*]|[\n]|([\*]+([^*/]|[\n])))*[\*]+\/ { } // remove multiline comments
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
	
	return tokenType;
}

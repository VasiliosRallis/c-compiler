#include <iostream>

#include "lexer.hpp"


// Define the instance of the variable that is declared in the histogram.hpp header
TokenValue yylval;

int main(){

    while(1){
        TokenType type=(TokenType)yylex();

        if(type == NONE){
            break; // No more tokens
	}
	else if(type == IDENTIFIER){
		delete yylval.wordValue;
	}
	else{
		//Do nothing
	}

    }

    return 0;
}

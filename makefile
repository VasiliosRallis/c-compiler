INCLUDE = -I include/ -I src/parser/ -I src/lexer/

CPPFLAGS += -W -Wall -g 

# This avoids error: ‘fileno’ was not declared in this scope
CPPFLAGS += -std=c++11

# Avoid warnings about yyunput not used
CPPFLAGS += -Wno-unused-function -Wno-sign-compare

# Indudes paths
CPPFLAGS += $(INCLUDE)

all : bin/c_compiler

bin/c_compiler : src/parser/parser.tab.o src/lexer/lexer.yy.o src/c_compiler.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o $@ $^
	
src/lexer/lexer.yy.cpp : src/lexer/lexer.flex src/parser/parser.tab.hpp
	flex -o $@ $<  
	
src/parser/parser.tab.cpp src/parser/parser.tab.hpp : src/parser/parser.y
	bison -v -d $< -o src/parser/parser.tab.cpp

clean :
	-rm -r bin
	-rm src/lexer/*.o
	-rm src/lexer/*.yy.cpp
	-rm src/parser/*.output
	-rm src/parser/*.o
	-rm src/parser/*.tab.*
	-rm -r tests/c/out
	-rm -r tests/python/out


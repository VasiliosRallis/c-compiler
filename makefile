CPPFLAGS += -W -Wall -g 

# This avoids error: ‘fileno’ was not declared in this scope
CPPFLAGS += -std=c++11

# Avoid warnings about yyunput not used
CPPFLAGS += -Wno-unused-function -Wno-sign-compare

# Indudes paths
CPPFLAGS += -I include/lexer

all : bin/lexer bin/print_canonical

src/lexer/lexer.yy.cpp : src/lexer/lexer.flex
	flex -o $@ $<  

bin/lexer : src/lexer/lexer.yy.o src/lexer/main.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o $@ $^

src/parser/parser.tab.cpp src/parser/parser.tab.hpp : src/parser/parser.y
	bison -v -d $< -o src/parser/parser.tab.cpp

bin/print_canonical : src/parser/print_canonical.o src/parser/parser.tab.o src/lexer/lexer.yy.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o $@ $^

clean :
	-rm -r bin
	-rm src/lexer/*.o
	-rm src/lexer/*.yy.cpp


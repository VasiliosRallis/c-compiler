CPPFLAGS += -W -Wall -g 

# This avoids error: ‘fileno’ was not declared in this scope
CPPFLAGS += -std=c++11

# Avoid warnings about yyunput not used
CPPFLAGS += -Wno-unused-function -Wno-sign-compare

# Indudes paths
CPPFLAGS += -I include/lexer

all : bin/lexer

src/lexer/lexer.yy.cpp : src/lexer/lexer.flex
	flex -o $@ $<  

bin/lexer : src/lexer/lexer.yy.o src/lexer/main.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o $@ $^

clean :
	-rm -r bin
	-rm src/lexer/*.o
	-rm src/lexer/*.yy.cpp


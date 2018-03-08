INCLUDE = -I include/ -I src/parser/ -I src/lexer/

SRC_DIR := src/ast
OBJ_DIR := src/ast
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

CPPFLAGS += -W -Wall -g

# This avoids error: ‘fileno’ was not declared in this scope
CPPFLAGS += -std=c++11

# Avoid warnings about yyunput not used
CPPFLAGS += -Wno-unused-function -Wno-sign-compare -Wno-unused-parameter

# Indudes paths
CPPFLAGS += $(INCLUDE)

all : bin/c_compiler

bin/c_compiler : src/parser/parser.tab.o src/lexer/lexer.yy.o src/c_compiler.o $(OBJ_FILES)
	mkdir -p bin
	g++ $(CPPFLAGS) -o $@ $^
	
src/lexer/lexer.yy.cpp : src/lexer/lexer.flex src/parser/parser.tab.hpp
	flex -o $@ $<  
	
src/parser/parser.tab.cpp src/parser/parser.tab.hpp : src/parser/parser.y
	bison -v -d $< -o src/parser/parser.tab.cpp
	
   
clean :
	rm -r tests/c/out
	rm -r tests/python/out
	rm -r tests/assemble/out
	find . -name "*.o" -type f -delete
	find . -name "*.exe" -type f -delete
	find . -name "*.tab.*" -type f -delete
	find . -name "*.output" -type f -delete
	find . -name "*.yy.cpp" -type f -delete

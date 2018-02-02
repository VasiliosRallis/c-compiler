#!/bin/bash

PARSER="tempName"

echo "========================================"
echo " Cleaning the temporaries and outputs"
make clean

echo "Building from scratch"
make all
if [[ "$?" -ne 0 ]]; then
	echo "Build failed.";
fi

mkdir -p tests/out

PASSED=0
CHECKED=0

for i in tests/in/*.c; do
    echo "==========================="
    echo ""
    echo "Input file : ${i}"
    BASENAME=$(basename $i .c);
    cat $i > tests/out/$BASENAME.c  2> tests/out/$BASENAME.stderr.txt #Must pipe the cat to the parser here
    
    # Compile the output of the output of the lexer
    
    make tests/out/$BASENAME
    
    if [[ "$?" -ne "0" ]]; then
        echo -e "\nERROR"
    else
        PASSED=$(( ${PASSED}+1 ));
    fi
    CHECKED=$(( ${CHECKED}+1 ));
    
    diff <(./tests/in/01; echo $?) <(./tests/out/01; echo $?) > tests/out/$BASENAME.diff.txt
    if [[ "$?" -ne "0" ]]; then
        echo -e "\nERROR"
    else
        PASSED=$(( ${PASSED}+1 ));
    fi
    CHECKED=$(( ${CHECKED}+1 ));
done

echo ""
echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo ""    

# This checks if the shell running is BASH
#!/bin/bash

PARSER="c_compiler"

echo "========================================"
echo "Cleaning the temporaries and outputs"
make clean
echo ""

echo "========================================"
echo "Building from scratch"
make all
if [[ "$?" -ne 0 ]]; then
	echo "Build failed.";
fi

mkdir -p tests/out



PASSED=0
CHECKED=0

# First test that all the test in test/in can actually compile
for i in tests/in/*.c; do
    BASENAME=$(basename $i .c);
    gcc -std=c89 -c tests/in/$BASENAME.c -o tests/in/$BASENAME.o
    if [[ "$?" -ne "0" ]]; then
        rm tests/in/*.o
        echo ""
        echo "ERROR: test ${BASENAME} is wrong!"
        exit 1
    fi
    rm tests/in/*.o
    
done

# Go through all the files pass them through the parser
for i in tests/in/*.c; do
    echo "==========================="
    echo ""
    echo "Input file : ${i}"
    BASENAME=$(basename $i .c);
    
    # The stderr will contain the loging info (useful for debugging)
    ./bin/$PARSER --parse tests/in/$BASENAME.c -o tests/out/$BASENAME.c 2> tests/out/$BASENAME.stderr.txt
    
    # Check if the parser actually exited with 0 (if it didn't the out.c will be empty which will actaully compile)
    if [[ "$?" -ne "0" ]]; then
    echo -e "\nERROR"
    else
        PASSED=$(( ${PASSED}+1 ));
        # Check if the parser output can compile
        gcc -std=c89 -c tests/out/$BASENAME.c -o tests/out/$BASENAME.o
    
        if [[ "$?" -ne "0" ]]; then
            echo -e "\nERROR"
        else
            PASSED=$(( ${PASSED}+1 ));
        fi
    fi
    
    CHECKED=$(( ${CHECKED}+2 ));

    
done

echo ""
echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo ""

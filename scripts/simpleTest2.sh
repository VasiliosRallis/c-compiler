# This checks if the shell running is BASH
#!/bin/bash

PARSER="c_compiler"

echo "Cleaning the temporaries and outputs"
echo "========================================"
make clean
echo ""

echo "Building from scratch"
echo "========================================"
make all
if [[ "$?" -ne 0 ]]; then
	echo "Build failed.";
	exit 1
fi
echo ""


PASSED=0
CHECKED=0

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Go through all the files pass them through the parser
echo "Running C parser tests"
echo "========================================"
mkdir -p tests/c/out

# First test that all the test in test/in can actually compile
for i in tests/c/in/*.c; do
    BASENAME=$(basename $i .c);
    gcc -std=c89 -w -c tests/c/in/$BASENAME.c -o tests/c/in/$BASENAME.o
    if [[ "$?" -ne "0" ]]; then
        rm tests/c/in/*.o
        echo ""
        echo "ERROR: test ${BASENAME} is wrong!"
        exit 1
    fi
    rm tests/c/in/*.o
    
done

for i in tests/c/in/*.c; do
#    echo "Input file : ${i}"
    BASENAME=$(basename $i .c);
    
    # The stderr will contain the loging info (useful for debugging)
    ./bin/$PARSER --parse tests/c/in/$BASENAME.c -o tests/c/out/$BASENAME.c 2> tests/c/out/$BASENAME.stderr.txt
    
    # Check if the parser actually exited with 0 (if it didn't the out.c will be empty which will actaully compile)
    if [[ "$?" -ne "0" ]]; then
    echo -e "${i} \t ${RED}PASS${NC} 0/2"
    else
        PASSED=$(( ${PASSED}+1 ));
        # Check if the parser output can compile
        gcc -std=c89 -w -c tests/c/out/$BASENAME.c -o tests/c/out/$BASENAME.o
    
        if [[ "$?" -ne "0" ]]; then
            echo -e "${i} \t ${RED}PASS${NC} 1/2"
        else
            echo -e "${i} \t ${GREEN}PASS${NC} 2/2"
            PASSED=$(( ${PASSED}+1 ));
        fi
    fi
    
    CHECKED=$(( ${CHECKED}+2 ));
    
done

P_RED=$(tput setaf 1)
P_GREEN=$(tput setaf 2)
P_NC=$(tput sgr 0)
RED_PASS="${P_RED}PASS${P_NC}"
GREEN_PASS="${P_GREEN}PASS${P_NC}"

echo ""
echo "Running Python translate tests"
echo "========================================"
mkdir -p tests/python/out
for i in tests/python/in/*.c; do
    BASENAME=$(basename $i .c);
    
    # The stderr will contain the loging info (useful for debugging)
    ./bin/$PARSER --translate tests/python/in/$BASENAME.c -o tests/python/out/$BASENAME.py 2> tests/python/out/$BASENAME.stderr.txt
    
    # Check if the parser actually exited with 0 (if it didn't the out.c will be empty which will actaully compile)
    if [[ "$?" -ne "0" ]]; then
        # echo -e "${i} \t ${RED}PASS${NC} 0/2"
        printf "%-60s ${RED_PASS} 1/2\n" "$i"
    else
        PASSED=$(( ${PASSED}+1 ));
  
        # Get the exit code of the reference python version
        python3 tests/python/in/$BASENAME.py
        REF_P_OUT=$?
    
        # Get the actual python exit code
        python3 tests/python/out/$BASENAME.py      
        GOT_P_OUT=$?
    
        if [[ $REF_P_OUT -ne $GOT_P_OUT ]]; then
            # echo -e "${i} \t ${RED}PASS${NC} 1/2"
            printf "%-60s ${RED_PASS} 1/2\n" "$i"
        else
            # echo -e "${i} \t ${GREEN}PASS${NC} 2/2"
            printf "%-60s ${GREEN_PASS} 2/2\n" "$i"
            PASSED=$(( ${PASSED}+1 ));
        fi
    
    fi
    
    CHECKED=$(( ${CHECKED}+2 ));
done

echo ""
echo "Running compiler to assembly tests"
echo "========================================================="
mkdir -p tests/assemble/out
for i in tests/assemble/in/*.c; do
    BASENAME=$(basename $i .c);
    
    # Compile the .c file and get the golden exit code
    gcc -std=c89 -w tests/assemble/in/$BASENAME.c -o tests/assemble/in/$BASENAME.exe
    if [[ "$?" -ne "0" ]]; then
        rm tests/assemble/in/*.exe
        echo ""
        echo "ERROR: test ${BASENAME} is wrong!"
        exit 1
    else
        ./tests/assemble/in/$BASENAME.exe
        REF_EXIT=$?
            
    ./bin/$PARSER -S tests/assemble/in/$BASENAME.c -o tests/assemble/out/$BASENAME.mips.s 2> tests/assemble/out/$BASENAME.stderr.txt  
    
    # Check if the parser actually exited with 0 (if it didn't the out.c will be empty which will actaully compile)
    if [[ "$?" -ne "0" ]]; then
        # echo -e "${i} \t ${RED}PASS${NC} 0/2"
        printf "%-60s ${RED_PASS} 0/2\n" "$i"
    else
        PASSED=$(( ${PASSED}+1 ));
  
            
            # Compile the .s file generated from our compiler
            mips-linux-gnu-gcc -w -std=c89 -march=mips1 -mfp32 -static -O0 tests/assemble/out/$BASENAME.mips.s -o tests/assemble/out/$BASENAME.mips.exe
            
            # Pass the .mips.exe through qemu and get exit code
            qemu-mips tests/assemble/out/$BASENAME.mips.exe
            
            # Compare exit codes
            GOT_EXIT=$?
            if [[ $REF_EXIT -ne $GOT_EXIT ]]; then
                # echo -e "${i} \t ${RED}PASS${NC} 1/2 \t REF_EXIT=${REF_EXIT} GOT_EXIT = ${GOT_EXIT}"
                printf "%-60s ${RED_PASS} 1/2 REF_EXIT=${REF_EXIT} GOT_EXIT = ${GOT_EXIT}\n" "$i"
            else
                # echo -e "${i} \t ${GREEN}PASS${NC} 2/2"
                printf "%-60s ${GREEN_PASS} 2/2\n" "$i"
                PASSED=$(( ${PASSED}+1 ));
            fi
    
        fi
    fi
    
    CHECKED=$(( ${CHECKED}+2 ));
done

echo ""
echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo ""

# The same as simpleTest2.sh but wihout the basic c tests

# This checks if the shell running is BASH
#!/bin/bash

PARSER="c_compiler"

echo "Cleaning the temporaries and outputs"
echo "========================================"
make clean
echo ""
rm -r tests/python/out
rm -r tests/assemble/out

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

P_RED=$(tput setaf 1)
P_GREEN=$(tput setaf 2)
P_NC=$(tput sgr 0)
RED_PASS="${P_RED}PASS${P_NC}"
GREEN_PASS="${P_GREEN}PASS${P_NC}"

if [ $1 != 'd' ]
    then 
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
    echo "Running assembly tests"
    echo "========================================================="
    mkdir -p tests/assemble/out
    for i in tests/assemble/in/*.c; do
        BASENAME=$(basename $i .c);
        
        # Compile the .c file and get the golden exit code
        gcc -std=c89 -w tests/assemble/in/$BASENAME.c -o tests/assemble/out/$BASENAME
        
        if [[ "$?" -ne "0" ]]; then
            echo ""
            echo "ERROR: test ${BASENAME} is wrong!"
            exit 1
        else
            ./tests/assemble/out/$BASENAME
            REF_EXIT=$?
                
            ./bin/$PARSER -S tests/assemble/in/$BASENAME.c -o tests/assemble/out/$BASENAME.mips.s 2> tests/assemble/out/$BASENAME.stderr.txt  

            # Check if the parser actually exited with 0 (if it didn't the out.c will be empty which will actaully compile)
            if [[ "$?" -ne "0" ]]; then
                # echo -e "${i} \t ${RED}PASS${NC} 0/2"
                printf "%-60s ${RED_PASS} 0/2\n" "$i"
            else
                PASSED=$(( ${PASSED}+1 ));
                  
                # Compile the .s file generated from our compiler
                mips-linux-gnu-gcc -w -std=c89 -march=mips1 -mfp32 -static -O0 tests/assemble/out/$BASENAME.mips.s -o tests/assemble/out/$BASENAME
                
                # Pass the .mips.exe through qemu and get exit code
                qemu-mips tests/assemble/out/$BASENAME
                
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
fi

echo ""
echo "Running assembly driver tests"
echo "========================================================="
mkdir -p tests/assemble/out_d
for i in tests/assemble/in_d/*.c; do
    
    # Make that the c file isn't the driver
    if [[ $i != *"_d.c" ]]; then
    
        BASENAME=$(basename $i .c);
        
        # Compile the .c file and get the golden exit code        
        gcc -std=c89 -w tests/assemble/in_d/$BASENAME.c tests/assemble/in_d/${BASENAME}_d.c -o tests/assemble/out_d/$BASENAME

        if [[ "$?" -ne "0" ]]; then
            echo ""
            echo "ERROR: test ${BASENAME} is wrong!"
            exit 1
        else
            ./tests/assemble/out_d/$BASENAME
            REF_EXIT=$?
                
        ./bin/$PARSER -S tests/assemble/in_d/$BASENAME.c -o tests/assemble/out_d/$BASENAME.mips.s 2> tests/assemble/out_d/$BASENAME.stderr.txt  
        
        # Check if the parser actually exited with 0 (if it didn't the out.c will be empty which will actaully compile)
        if [[ "$?" -ne "0" ]]; then
            # echo -e "${i} \t ${RED}PASS${NC} 0/2"
            printf "%-60s ${RED_PASS} 0/2\n" "$i"
        else
            PASSED=$(( ${PASSED}+1 ));

            # Compile the .s file generated from our compiler
            mips-linux-gnu-gcc -w -std=c89 -march=mips1 -mfp32 -static -O0 tests/assemble/out_d/$BASENAME.mips.s tests/assemble/in_d/${BASENAME}_d.c -o tests/assemble/out_d/$BASENAME
            
            # Pass the binary through qemu and get exzit code
            qemu-mips tests/assemble/out_d/$BASENAME
            
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
    fi

done

echo ""
echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo ""


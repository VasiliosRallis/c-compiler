# This checks if the shell running is BASH
#!/bin/bash

PARSER="c_compiler"

echo "Cleaning the temporaries and outputs"
echo "========================================"
make clean
echo ""
rm -r test_deliverable/output

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

P_RED=$(tput setaf 1)
P_GREEN=$(tput setaf 2)
P_NC=$(tput sgr 0)
RED_FAIL="${P_RED}FAIL${P_NC}"
GREEN_PASS="${P_GREEN}PASS${P_NC}"


echo ""
echo "Running assembly driver tests"
echo "========================================================="
mkdir -p test_deliverable/output
for i in test_deliverable/test_cases/*.c; do
    
    # Make that the c file isn't the driver
    if [[ $i != *"_driver.c" ]]; then
    
        BASENAME=$(basename $i .c);
        
        # Compile the .c file and get the golden exit code        
        gcc -std=c89 -w test_deliverable/test_cases/$BASENAME.c test_deliverable/test_cases/${BASENAME}_driver.c -o test_deliverable/output/$BASENAME

        if [[ "$?" -ne "0" ]]; then
            echo ""
            echo "ERROR: test ${BASENAME} is wrong!"
            exit 1
        else
            ./test_deliverable/output/$BASENAME
            REF_EXIT=$?
                
            ./bin/$PARSER -S test_deliverable/test_cases/$BASENAME.c -o test_deliverable/output/$BASENAME.mips.s 2> test_deliverable/output/$BASENAME.stderr.txt  
            

            # Compile the .s file generated from our compiler
            mips-linux-gnu-gcc -w -std=c89 -march=mips1 -mfp32 -static -O0 test_deliverable/output/$BASENAME.mips.s test_deliverable/test_cases/${BASENAME}_driver.c -o test_deliverable/output/$BASENAME
            
            # Pass the binary through qemu and get exit code
            qemu-mips test_deliverable/output/$BASENAME
            
            # Compare exit codes
            GOT_EXIT=$?
            if [[ $REF_EXIT -ne $GOT_EXIT ]]; then
                printf "%-50s ${RED_FAIL} REF_EXIT=${REF_EXIT} GOT_EXIT = ${GOT_EXIT}\n" "$i"
            else
                printf "%-50s ${GREEN_PASS}\n" "$i"
                PASSED=$(( ${PASSED}+1 ));
            fi
        
        fi
        
        CHECKED=$(( ${CHECKED}+1 ));
    fi

done

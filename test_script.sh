#!/bin/bash

for f in Test/hencode_ex
    do echo Run $f && ~ngonella/public/csc-357/asgn3/hencode $f
done > expect.txt

for f in Test/hencode_ex
    do echo Run $f && ./h_encode $f
    if [ $? -eq 139 ]; then
        echo "Crashed at $f"
        exit 1
    fi
done > out.txt

diff expect.txt out.txt > diff.txt | cat

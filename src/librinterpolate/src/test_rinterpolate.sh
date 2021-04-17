#!/bin/bash

STACK="--main-stacksize=300000000 --max-stackframe=1600006240"

VALGRIND="valgrind $STACK --read-var-info=yes --track-origins=yes --tool=memcheck --leak-check=full --show-reachable=yes --freelist-vol=100000000 --partial-loads-ok=no --undef-value-errors=yes -v --vgdb-error=1"

CALLGRIND="valgrind $STACK --tool=callgrind --simulate-cache=yes --dump-instr=yes --trace-jump=yes --callgrind-out-file=callgrind.out"

CACHEGRIND="valgrind $STACK --tool=cachegrind --cachegrind-out-file=cachegrind.out"

export LD_LIBRARY_PATH=.
#$VALGRIND ./test_rinterpolate
#$CALLGRIND ./test_rinterpolate
#$CACHEGRIND ./test_rinterpolate
#gdb test_rinterpolate
time ./test_rinterpolate

#!/bin/bash

#
# Script to build using PGO
#
# This has little effect in my experience, but you should try it.
#

# compiler choice:
#
# use gcc by default 
export CC="gcc"

# or clang (set version number manually)
V="7"
#export CC="clang-$V"


TEST="test_rinterpolate.sh | grep With"
MAKE="make -j"
CLEAN="make clean"
DIR=.pgo
rm -rf $DIR
mkdir -p $DIR

if [[ $CC =~ clang ]]; then
    # clang
    CLANGMERGE="llvm-profdata-$V"
    GENERATE="-fprofile-generate=$DIR"
    USE="-fprofile-use=$DIR/code.profdata"
    MID="$CLANGMERGE merge -output=$DIR/code.profdata $DIR"
else
    # gcc (default)
    GENERATE="-fprofile-generate -fprofile-dir=$DIR"
    USE="-fprofile-use -fprofile-dir=$DIR"
    MID=""
fi

export COPTFLAGS=$GENERATE
$CLEAN
$MAKE
echo "Running quietly... please wait"
$TEST > /dev/null

$MID

export COPTFLAGS=$USE
$CLEAN
$MAKE
$TEST

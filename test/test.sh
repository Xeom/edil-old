#!/bin/bash

realfile=/tmp/edil-test-real
simfile=/tmp/edil-test-sim
difffile=/tmp/edil-test-diff

function COL_D {
    printf "\x1b[0;%dm" $1
}

function COL_B {
    printf "\x1b[1;%dm" $1
}

function COL_END {
    printf "\x1b[0m"
}

function BRACKET {
    echo $(COL_B $1)\[$(COL_D $1)$2$(COL_B $1)\]$(COL_END)
}

function run_test {
    if [[ ! -d $1 ]]; then
        echo $(BRACKET 33 w) "$1 not a directory."
        return
    fi

    make -C.. test/$1/test.out

    if [[ $? == 2 ]]; then
        echo $(BRACKET 31 !) "Error making $1 test binary."
        make -C.. clean_test
        exit -1
    fi

    echo $(BRACKET 35 i) "Running tests for $1 ..."

    $1/test.out > $realfile &
    testpid=$!

    python3 $1/sim.py > $simfile
    simrtn=$?

    wait $testpid

    if [[ $? != 0 ]]; then
        echo $(BRACKET 31 !) "Error testing $1."
        exit -1
    fi

    if [[ $simrtn != 0 ]]; then
        echo $(BRACKET 31 !) "Error simulating $1."
        exit -1
    fi

    diff $realfile $simfile > $difffile

    if [[ -s $difffile ]]; then
        echo $(BRACKET 33 w) "Difference found in results of $1 tests"
        echo $(BRACKET 35 i) "Saving diff to diff/$1"

        [[ -d diff ]] || mkdir diff
        cp $difffile diff/$1
    else
        echo $(BRACKET 32 .) "Finished testing $1."
    fi

    rm $realfile
    rm $simfile
    rm $difffile
}

[[ -d diff ]] && rm diff/*

if [[ $# == 0 ]]; then
    files=unit_*
else
    files=$*
fi

for i in $files; do
    run_test ${i%/}
done

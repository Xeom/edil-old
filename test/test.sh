#!/bin/bash

realfile=/tmp/edil-test-real
simfile=/tmp/edil-test-sim
difffile=/tmp/edil-test-diff

pinfo() {
    ../bracket.sh 35 i  1>&2
    printf " %s\n" "$1" 1>&2
}

pwarn() {
    ../bracket.sh 33 w  1>&2
    printf " %s\n" "$1" 1>&2
}

perr() {
    ../bracket.sh 31 !  1>&2
    printf " %s\n" "$1" 1>&2
    exit -1
}

psucc() {
    ../bracket.sh 32 .  1>&2
    printf " %s\n" "$1" 1>&2
}

function run_test {
    if [[ ! -d $1 ]]; then
        pwarn "$1 not a directory."
        return
    fi

    make -C.. test/$1/test.out

    if [[ $? == 2 ]]; then
        perr "Error making $1 test binary."
        make -C.. clean_test
        exit -1
    fi

    pinfo "Running tests for $1 ..."

    $1/test.out > $realfile &
    testpid=$!

    python3 $1/sim.py > $simfile
    simrtn=$?

    wait $testpid

    if [[ $? != 0 ]]; then
        perr "Error testing $1."
    fi

    if [[ $simrtn != 0 ]]; then
        perr "Error simulating $1."
    fi

    pinfo "Comparing simulated and real outputs"
    diff -I "LINE: *" -B $realfile $simfile > $difffile

    if [[ -s $difffile ]]; then
        pwarn "Difference found in results of $1 tests"
        pinfo "Saving diff to diff/$1, diff/$1.sim diff/$1.real"

        [[ -d diff ]] || mkdir diff

        cp $difffile diff/$1
        cp $realfile diff/$1.real
        cp $simfile  diff/$1.sim
    else
        psucc "Finished testing $1."
    fi

    rm $realfile
    rm $simfile
    rm $difffile
}

[[ -d diff ]] && rm -r diff

if [[ $# == 0 ]]; then
    files=unit_*
else
    files=$*
fi

for i in $files; do
    run_test ${i%/}
done

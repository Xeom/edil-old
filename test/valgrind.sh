#!/bin/sh
. ./pstatus.sh

valfile=/tmp/edil-test-val
valflags="--error-exitcode=128 --leak-check=full --log-file=$valfile"

run_test() {
    if [ ! -d $1 ]; then
        pwarn "$1 not a directory."
        return
    fi

    make -C.. DEFINES="TEST_REDUCED NOPRINT" test/$1/test.out

    if [ $? -eq 2 ]; then
        perr "Error making $1 test binary."
        make -C.. clean_test
        exit -1
    fi

    pinfo "Running valgrind for $1 ..."

    valgrind $valflags $1/test.out >/dev/null

    if [ $? -ne 0 ]; then
        pwarn "Errors detected in $1 test."
        pinfo "Saving errors to val/$1."

        [ -d val ] || mkdir val

        cp $valfile val/$1

    else
        psucc "Finished testing $1."
    fi

    rm $valfile
}

make -C.. clean_test

[ -d val ] && rm -r val

if [ $# -eq 0 ]; then
    files=unit_*
else
    files=$*
fi

for i in $files; do
    run_test ${i%/}
done

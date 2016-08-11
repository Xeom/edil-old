#!/bin/sh

valfile=/tmp/edil-test-val
valflags="--error-exitcode=128 --leak-check=full --log-file=$valfile"

run_test() {
    if [ ! -d $1 ]; then
        ../misc/plog.sh warn "$1 not a directory."
        return
    fi

    make -C.. DEFINES="TEST_REDUCED NOPRINT" test/$1/test.out

    if [ $? -eq 2 ]; then
        ../misc/plog.sh err "Error making $1 test binary."
        make -C.. clean_test
        exit -1
    fi

    ../misc/plog.sh info "Running valgrind for $1 ..."

    valgrind $valflags $1/test.out >/dev/null

    if [ $? -ne 0 ]; then
        ../misc/plog.sh warn "Errors detected in $1 test."
        ../misc/plog.sh info "Saving errors to val/$1."

        [ -d val ] || mkdir val

        cp $valfile val/$1

    else
        ../misc/plog.sh succ "Finished testing $1."
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

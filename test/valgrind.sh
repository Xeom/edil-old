#!/bin/sh

rootdir=$(dirname $(dirname $0))

valfile=/tmp/edil-test-val
valflags="--error-exitcode=128 --leak-check=full --log-file=$valfile"

run_test() {
    if [ ! -d $1 ]; then
        $rootdir/misc/plog.sh warn "$1 not a directory."
        return
    fi

    defines="TEST_REDUCED NOPRINT ROOTDIR=$rootdir"
    make -C$rootdir DEFINES="$defines" test/$(basename $1)/test.out

    if [ $? -eq 2 ]; then
        $rootdir/misc/plog.sh err "Error making $1 test binary."
        make -C$rootdir clean_test
        exit -1
    fi

    $rootdir/misc/plog.sh info "Running valgrind for $1 ..."

    valgrind $valflags $1/test.out >/dev/null

    if [ $? -ne 0 ]; then
        $rootdir/misc/plog.sh warn "Errors detected in $1 test."
        $rootdir/misc/plog.sh info "Saving errors to val/$1."

        [ -d val ] || mkdir val

        cp $valfile val/$1

    else
        $rootdir/misc/plog.sh succ "Finished testing $1."
    fi

    rm $valfile
}

make -C$rootdir clean_test

[ -d val ] && rm -r val

if [ $# -eq 0 ]; then
    files=$rootdir/test/unit_*
else
    files=$*
fi

for i in $files; do
    run_test ${i%/}
done

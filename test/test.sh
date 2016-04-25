function run_test {
    make -C.. test/$1/test.out
    if [[ $? == 2 ]] ; then
        echo "Error making $1 test binary"
    fi

    echo "Running tests for $1"

    diff <(./$1/test.out) <(python3 ./$1/sim.py)

    echo "Run tests"
}


for i in $*; do
    run_test $i
done

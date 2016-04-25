TEST_LEN = 100000

def test_print_vec(v):
    for ind, item in enumerate(v):
        print("ind ->", ind)
        print("*(int *)vec_item(v, ind) ->", item)

def test_insert_end():
    v = []

    for ind in range(TEST_LEN):
        inv = -ind

        print("ind ->", ind)
        print("inv ->", inv)
        print("vec_len(v) ->", len(v))
        v.insert(ind, inv)
        print("vec_insert(v, vec_len(v), 1, &inv) -> 0")

    test_print_vec(v)

def test_insert_start():
    v = []

    for ind in range(TEST_LEN):
        inv = -ind

        print("ind ->", ind)
        print("inv ->", inv)
        print("vec_len(v) ->", len(v))
        v.insert(0, inv)
        print("vec_insert(v, 0, 1, &inv) -> 0")

    test_print_vec(v)

test_insert_end()
test_insert_start()


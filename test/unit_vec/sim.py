TEST_LEN = 100000

def test(*args):
    print("LINE: N/A")
    print(*args)
    print("")

def test_print_vec(v):
    for ind, item in enumerate(v):
        test("ind ->", ind)
        test("*(int *)vec_item(v, ind) ->", item)

def test_insert_end():
    v = []

    for ind in range(TEST_LEN):
        inv = -ind

        test("ind ->", ind)
        test("inv ->", inv)
        test("vec_len(v) ->", len(v))

        v.insert(ind, inv)

        test("vec_insert(v, vec_len(v), 1, &inv) -> 0")

    test_print_vec(v)

def test_insert_start():
    v = []

    for ind in range(TEST_LEN):
        inv = -ind

        test("ind ->", ind)
        test("inv ->", inv)
        test("vec_len(v) ->", len(v))

        v.insert(0, inv)

        test("vec_insert(v, 0, 1, &inv) -> 0")

    test_print_vec(v)

test_insert_end()
test_insert_start()


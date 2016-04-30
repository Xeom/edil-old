TEST_LEN = 20000
TEST_FIND_LEN = 2000

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

        test("vec_insert(v, ind, 1, &inv) -> 0")

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

def test_for():
    v = []

    for ind in range(TEST_LEN):
        v.insert(ind, ind)

    for index, item in enumerate(v):
        test("_vec_index ->", index)
        test("item ->", item)

    for item in reversed(v):
        test("_vec_index ->", item)
        test("item ->", item)

def test_del():
    v = []

    for ind in range(TEST_LEN):
        v.insert(ind, ind)

    ind = 0
    while ind < len(v):
        if not (ind // 20 % 2 and ind % 7 and ind % 11):
            test("ind ->", ind)
            test("vec_delete(v, ind, 1) -> 0")
            del v[ind]
            test("vec_len(v) ->", len(v))

        ind += 1

    test_print_vec(v)

    while v:
        test("vec_delete(v, 0, 1) -> 0")
        del v[0]
        test("vec_len(v) ->", len(v))

    test_print_vec(v)

def test_find():
    v = []

    for ind in range(TEST_FIND_LEN):
        v.insert(ind, ind)

    for item in range(TEST_FIND_LEN):
        test("item ->", item)
        test("vec_find(v, &item) ->", v.index(item))
        test("vec_rfind(v, &item) ->", v.index(item))
        test("vec_contains(v, &item) -> 1")

    test("item -> -1")
    test("vec_contains(v, &item) -> 0")
    test("item ->", TEST_FIND_LEN + 1)
    test("vec_contains(v, &item) -> 0")

test_insert_end()
test_insert_start()
test_for()
test_del()
test_find()

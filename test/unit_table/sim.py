TEST_LEN = 46341

def test(*args):
    print("LINE: N/A")
    print(*args)
    print("")

def test_print_table(tbl):
    for ind in range(1, TEST_LEN):
        val = tbl.get(ind)
        test("ind ->", ind)

        if val == None:
            test("table_get(tbl, &ind) -> NULL")

        else:
            test("*(int *)table_get(tbl, &ind) ->", val)

def test_init():
    test("tbl == NULL -> 0")
    test("tbl == NULL -> 0")
    test("tbl == NULL -> 0")

def test_set():
    tbl = {}

    for ind in range(1, TEST_LEN):
        sqr = ind ** 2
        test("ind ->", ind)
        test("sqr ->", sqr)
        tbl[ind] = sqr
        test("table_set(tbl, &ind, &sqr) -> 0")

    test_print_table(tbl)

def test_del():
    tbl = {}

    for ind in range(1, TEST_LEN):
        test("ind ->", ind)
        tbl[ind] = ind
        test("table_set(tbl, &ind, &ind) -> 0")
        test("table_len(tbl) ->", len(tbl))

    for ind in range(1, TEST_LEN):
        if (ind // 20) % 2 and ind % 7 and ind %11:
            continue

        test("ind ->", ind)
        del tbl[ind]
        test("table_delete(tbl, &ind) -> 0")
        test("table_len(tbl) ->", len(tbl))

    test_print_table(tbl)

    for ind in range(1, TEST_LEN):
        if (ind // 20) % 2 and ind % 7 and ind %11:
            test("ind ->", ind)
            del tbl[ind]
            test("table_delete(tbl, &ind) -> 0")
            test("table_len(tbl) ->", len(tbl))

    test_print_table(tbl)

test_init()
test_set()
test_del()

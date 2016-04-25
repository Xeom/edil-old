TEST_LEN = 46341
a
def test_print_table(tbl):
    for ind in range(1, TEST_LEN):
        val = tbl.get(ind)
        print("ind ->", ind)

        if val == None:
            print("table_get(tbl, &ind) -> NULL")

        else:
            print("*(int *)table_get(tbl, &ind) ->", val)

def test_set():
    tbl = {}

    for ind in range(1, TEST_LEN):
        sqr = ind ** 2
        print("ind ->", ind)
        print("sqr ->", sqr)
        tbl[ind] = sqr
        print("table_set(tbl, &ind, &sqr) -> 0")

    test_print_table(tbl)

def test_del():
    tbl = {}

    for ind in range(1, TEST_LEN):
        print("ind ->", ind)
        tbl[ind] = ind
        print("table_set(tbl, &ind, &ind) -> 0")

    for ind in range(1, TEST_LEN):
        if (ind // 20) % 2 and ind % 7 and ind %11:
            continue

        print("ind ->", ind)
        del tbl[ind]
        print("table_delete(tbl, &ind) -> 0")

    test_print_table(tbl)

    for ind in range(1, TEST_LEN):
        if (ind // 20) % 2 and ind % 7 and ind %11:
            print("ind ->", ind)
            del tbl[ind]
            print("table_delete(tbl, &ind) -> 0")

    test_print_table(tbl)

test_set()
test_del()

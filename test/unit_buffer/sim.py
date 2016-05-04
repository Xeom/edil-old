TEST_LEN = 20000

def test_print_buffer(b):
    for ind in range(len(b)):
        print("ind ->", ind)

        print("(char *)vec_item(l, 0) ->", b[ind])

def test_insert_end():
    b = []

    for ind in range(TEST_LEN):
        print("ind ->", ind)
        print("buffer_insert(b, ind) ->", 0)
        b.insert(ind, None)
        print("buffer_set_line(b, ind, v) ->", 0)
        b[ind] = str(ind)

    test_print_buffer(b)

def test_insert_start():
    b = []

    for ind in range(TEST_LEN):
        print("ind ->", ind)
        print("buffer_insert(b, 0) ->", 0)
        b.insert(0, None)
        print("buffer_set_line(b, 0, v) ->", 0)
        b[0] = str(ind)

    test_print_buffer(b)

test_insert_end()
test_insert_start()

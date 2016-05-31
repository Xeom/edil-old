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
        print("buffer_len(b) ->", len(b))

    test_print_buffer(b)

def test_insert_start():
    b = []

    for ind in range(TEST_LEN):
        print("ind ->", ind)
        print("buffer_insert(b, 0) ->", 0)
        b.insert(0, None)
        print("buffer_set_line(b, 0, v) ->", 0)
        b[0] = str(ind)
        print("buffer_len(b) ->", len(b))

    test_print_buffer(b)

def test_insert_middle():
    b = []

    for ind in range(TEST_LEN):
        print("ind ->", ind)
        print("buffer_insert(b, ind / 2) ->", 0)
        b.insert(ind // 2, None)
        print("buffer_set_line(b, ind / 2, v) ->", 0)
        b[ind // 2] = str(ind)
        print("buffer_len(b) ->", len(b))

    test_print_buffer(b)

def test_init():
    print("b == NULL ->", 0)

def test_del():
    b = []

    for ind in range(TEST_LEN):
        b.insert(0, None)
        b[0] = str(ind)

    ind = 0
    while ind < len(b):
        if not ((ind // 20) % 2 and ind % 7 and ind % 11):
            print("ind ->", ind)
            print("buffer_delete(b, ind) ->", 0)
            del b[ind]
            print("buffer_len(b) ->", len(b))

        ind += 1

    test_print_buffer(b)

    while len(b):
        print("buffer_delete(b, 0) ->", 0)
        del b[0]
        print("buffer_len(b) ->",  len(b))

    test_print_buffer(b)

test_insert_end()
test_insert_start()
test_insert_middle()
test_init()
test_del()

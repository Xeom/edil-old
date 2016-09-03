def test_symbol():
    print("test_hexdump(teststr1, sizeof teststr1, buffer) ->",
          "61 73 63 69 69 20 61 00")
    print("test_hexdump(teststr1, sizeof teststr1, buffer) ->",
          "01 01 01 01 01 01 01 01")
    print("test_hexdump(teststr2, sizeof teststr2, buffer) ->",
          "75 74 66 38 5f 32 77 69 64 65 20 c1 81 00")
    print("test_hexdump(teststr2, sizeof teststr2, buffer) ->",
          "01 01 01 01 01 01 01 01 01 01 01 02 82 01")
    print("test_hexdump(teststr3, sizeof teststr3, buffer) ->",
          "75 74 66 38 5f 33 77 69 64 65 20 e1 81 81 00")
    print("test_hexdump(teststr3, sizeof teststr3, buffer) ->",
          "01 01 01 01 01 01 01 01 01 01 01 03 82 82 01")
    print("test_hexdump(teststr4, sizeof teststr4, buffer) ->",
          "75 74 66 38 5f 34 77 69 64 65 20 f1 81 81 81 00")
    print("test_hexdump(teststr4, sizeof teststr4, buffer) ->",
          "01 01 01 01 01 01 01 01 01 01 01 04 82 82 82 01")
    print("test_hexdump(teststr5, sizeof teststr5, buffer) ->",
          "66 61 63 65 5f 73 74 61 72 74 20 0a 43 55 52 53 41 00")
    print("test_hexdump(teststr5, sizeof teststr5, buffer) ->",
          "01 01 01 01 01 01 01 01 01 01 01 81 01 01 01 01 01 01")
    print("test_hexdump(teststr6, sizeof teststr6, buffer) ->",
          "75 74 66 38 5f 62 69 67 20 20 20 fe 81 81 81 81 81 81 00")
    print("test_hexdump(teststr6, sizeof teststr6, buffer) ->",
          "01 01 01 01 01 01 01 01 01 01 01 83 82 82 82 82 82 82 01")

test_symbol()


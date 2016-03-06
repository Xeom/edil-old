import unittest

from cinterface.vec import Vec
import ctypes

class TestVecWithInts(unittest.TestCase):
    def setUp(self):
        self.v = Vec(ctypes.c_int)
        self.v.init()

    def assertcontents(self, *values):
        self.assertEqual([i.value for i in self.v], list(values))

    def push500(self):
        for i in range(500):
            self.v.push(i)

    def test_push(self):
        for i in range(500):
            self.assertcontents(*range(i))
            self.v.push(i)

    def test_get(self):
        self.push500()

        for i in range(500):
            self.assertEqual(self.v.get(i).value, i)

    def test_pop(self):
        self.push500()

        for i in range(500)[::-1]:
            self.v.pop()
            self.assertcontents(*range(i))

    def test_alloc(self):
        for i in range(50000):
            self.v.push(i)

        self.assertGreaterEqual(self.v.struct.contents.capacity, ctypes.sizeof(ctypes.c_int) * 50000)

        for i in range(50000):
            self.v.pop()

        self.assertGreater(100, self.v.struct.contents.capacity)

    def test_set(self):
        self.push500()

        for i in range(500):
            self.v.set(i, i**2)

        self.assertcontents(*(i**2 for i in range(500)))

    def test_trim(self):
        self.push500()

        self.v.trim(250)

        self.assertcontents(*range(250))

    def test_slice_from_start(self):
        self.push500()

        self.v = self.v.slice(0, 250)

        self.assertcontents(*range(250))

    def test_slice_from_end(self):
        self.push500()

        self.v = self.v.slice(250, 500)

        self.assertcontents(*(250 + i for i in range(250)))

    def test_slice_from_middle(self):
        self.push500()

        self.v = self.v.slice(100, 400)

        self.assertcontents(*range(100, 400))

    def test_append(self):
        other = Vec(ctypes.c_int)
        other.init()

        self.push500()

        for i in range(500):
            other.push(i + 500)

        self.v.append(other)
        self.assertcontents(*range(1000))

    def test_len(self):
        self.push500()

        self.assertEqual(len(self.v), 500)

    def test_find(self):
        self.push500()

        for i in range(500):
            self.assertEqual(self.v.find(i), i)

    def test_rfind(self):
        self.push500()

        for i in range(500):
            self.assertEqual(self.v.rfind(i), i)

    def test_insert_start(self):
        self.push500()

        self.v.insert(0, 1000)
        self.assertcontents(1000, *range(500))

    def test_insert_end(self):
        self.push500()

        self.v.insert(500, 1000)
        args = list(range(500))
        args.append(1000)
        self.assertcontents(*args)

    def test_insert_middle(self):
        self.push500()

        self.v.insert(250, 1000)
        args = list(range(500))
        args.insert(250, 1000)
        self.assertcontents(*args)

    def test_remove_start(self):
        self.push500()

        self.v.remove(0)
        self.assertcontents(*range(1, 500))

    def test_remove_end(self):
        self.push500()

        self.v.remove(499)
        self.assertcontents(*range(499))

    def test_remove_middle(self):
        self.push500()

        self.v.remove(250)
        args = list(range(250)) + list(range(251, 500))
        self.assertcontents(*args)

    def test_delete_start(self):
        self.push500()

        self.v.delete(0)
        self.assertcontents(*range(1, 500))

    def test_delete_end(self):
        self.push500()

        self.v.delete(499)
        self.assertcontents(*range(499))

    def test_delete_middle(self):
        self.push500()

        self.v.delete(250)
        args = list(range(250)) + list(range(251, 500))
        self.assertcontents(*args)


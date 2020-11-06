from subprocess import Popen, PIPE
import random

import unittest


def get_random_string(min_len: int = 10, max_len: int = 15) -> str:
    result_len = random.randint(min_len, max_len)

    characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

    result_string = ""
    for _ in range(result_len):
        result_string += characters[random.randint(0, len(characters) - 1)]

    return result_string


class TestClient:

    class ExecutionError(RuntimeError):
        pass

    def __init__(self, name):
        self.name = name

    @staticmethod
    def raise_for_exit_code(exit_code: int, err: bytes):
        raise(TestClient.ExecutionError(
            "Command returnred with non zero exit code %d\n%s" % (exit_code, err.decode())))

    def execute(self, cmd: str, *args) -> str:
        process = Popen(["./client", self.name, cmd, *args],
                        stdout=PIPE, stderr=PIPE)
        output, err = process.communicate()
        exit_code = process.wait()

        return exit_code, output, err

    def find(self, key):
        exit_code, output, err = self.execute("find", key)

        if exit_code != 0 and err != b"":
            TestClient.raise_for_exit_code(exit_code, err)

        if exit_code != 0:
            return None

        return output.decode().split("\n")[0]

    def insert(self, key, value):
        exit_code, _, err = self.execute("insert", key, value)

        if exit_code != 0:
            TestClient.raise_for_exit_code(exit_code, err)

    def drop(self):
        exit_code, _, err = self.execute("drop")

        if exit_code != 0:
            TestClient.raise_for_exit_code(exit_code, err)

    def erase(self, key):
        exit_code, _, err = self.execute("erase", key)

        if exit_code != 0 and err != b"":
            TestClient.raise_for_exit_code(exit_code, err)

        return exit_code == 0

    def size(self):
        exit_code, output, err = self.execute("size")

        if exit_code != 0:
            TestClient.raise_for_exit_code(exit_code, err)

        return int(output.decode().split("\n")[0])


class BasicFunctionality(unittest.TestCase):

    object_name = "obj1"

    def setUp(self):
        self.client = TestClient(BasicFunctionality.object_name)
        self.client.drop()

    def tearDown(self):
        self.client.drop()

    def test_read_write(self):
        k1 = get_random_string()
        k2 = get_random_string()

        v1 = get_random_string()
        v2 = get_random_string()

        self.assertIsNone(self.client.find(k1))
        self.assertIsNone(self.client.find(k2))

        self.client.insert(k1, v1)
        self.assertEqual(v1, self.client.find(k1))

        self.client.insert(k1, v2)
        self.assertEqual(v2, self.client.find(k1))

    def test_compare_to_dict(self):

        reference_dict = dict()

        key_pool = [get_random_string() for _ in range(100)]
        value_pool = [get_random_string() for _ in range(1500)]

        def ins(k, v):
            reference_dict[k] = v
            self.client.insert(k, v)

        def rm(k, _):
            r1 = reference_dict.pop(k, None)
            r2 = self.client.erase(k)

            self.assertEqual(r1 != None, r2)

        for _ in range(4000):
            act = random.choices([ins, rm], weights=[5, 1])[0]

            k = random.choice(key_pool)
            v = random.choice(value_pool)

            act(k, v)

        self.assertEqual(len(reference_dict.keys()), self.client.size())

        for ki, vi in reference_dict.items():
            vii = self.client.find(ki)

            self.assertEqual(vi, vii)


if __name__ == '__main__':
    unittest.main()

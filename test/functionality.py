from subprocess import Popen, PIPE
import random

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
        raise(TestClient.ExecutionError("Command returnred with non zero exit code %d\n%s" % (exit_code, err.decode())))


    def execute(self, cmd: str, *args) -> str:
        process = Popen(["./client", self.name, cmd, *args], stdout=PIPE, stderr=PIPE)
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
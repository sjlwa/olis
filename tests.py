import unittest
from subprocess import Popen, PIPE

from os import getuid
from pwd import getpwuid


prompt = getpwuid(getuid()).pw_name + "> "


def run_script(script):
    output = None
    with Popen(
      ["./olis"],
      stdin=PIPE,
      stdout=PIPE,
      stderr=PIPE,
      text=True) as pipe:

        for command in script:
            pipe.stdin.write(command + '\n')

        pipe.stdin.close()
        output = pipe.stdout.read()

    return output.split('\n')


class TestOlis(unittest.TestCase):
    def test_printing(self):
        output = run_script(["just print the input", "asdasd"])

        real_output = ["just print the input", "asdasd"]
        real_output = [prompt + string for string in real_output]
        real_output.append(prompt)

        self.assertEqual(
          real_output,
          output)


if __name__ == "__main__":
    unittest.main()

from asyncio import subprocess
from typing import *
import os
import subprocess


class BwtCoder:
    def __init__(self, path_to_binary=None) -> None:
        self.dir = os.path.dirname(os.path.realpath(__file__))
        if path_to_binary is not None:
            self.binary = path_to_binary
        else:
            self.binary = os.path.join(self.dir, "bwt")

    def encode(self, filepath, out_name="bwt_encoded") -> str:
        out = os.path.join(self.dir, out_name)
        self._call(["encode", filepath, out])
        return out

    def decode(self, encoded, decoded):
        self._call(["decode", encoded, decoded])

    def _call(self, args):
        process = subprocess.Popen([f"./{self.binary}", *args], shell=False)
        errcode = process.wait()
        if errcode != 0:
            out, err = process.communicate()
            print("Failed to use bwt.")
            print(str(out))
            print(str(err))

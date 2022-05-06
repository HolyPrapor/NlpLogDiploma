from asyncio import subprocess
from typing import *
import os
import subprocess


class BwtCoder:
    def __init__(self, path_to_binary=None) -> None:
        if path_to_binary is not None:
            self.binary = path_to_binary
        else:
            filedir = os.path.dirname(os.path.realpath(__file__))
            self.binary = os.path.join(filedir, "bwt")

    def encode(self, encode, bwt_encoded):
        self._call(["encode", encode, bwt_encoded])

    def decode(self, bwt_encoded, bwt_decoded):
        self._call(["decode", bwt_encoded, bwt_decoded])

    def _call(self, args):
        process = subprocess.Popen([self.binary, *args], shell=False)
        errcode = process.wait()
        if errcode != 0:
            out, err = process.communicate()
            print(f"Failed to use bwt. Error code: {errcode}")
            print(str(out))
            print(str(err))

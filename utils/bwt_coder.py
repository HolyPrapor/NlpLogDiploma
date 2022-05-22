from asyncio import subprocess
from collections import Counter
import os
import subprocess
from typing import Tuple, List


class BwtCoder:
    def __init__(self, path_to_binary=None) -> None:
        if path_to_binary is not None:
            self.binary = path_to_binary
        else:
            filedir = os.path.dirname(os.path.realpath(__file__))
            self.binary = os.path.join(filedir, "bwt")

    @staticmethod
    def encode_eof_token(input_file_name: str) -> Tuple[bytes, bytes, List[int]]:
        with open(input_file_name, "rb") as f:
            text = f.read()
        eof_byte = None
        indexes = []
        for i in range(256):
            current_byte = bytes(i.to_bytes(1, "big"))
            if text.find(current_byte) == -1:
                eof_byte = current_byte
                break
        if not eof_byte:
            counter = Counter(text)
            eof_byte = bytes(counter.most_common()[-1][0].to_bytes(1, "big"))
            for i, byte in enumerate(text):
                if bytes(byte.to_bytes(1, "big")) == eof_byte:
                    indexes.append(i)
        if indexes:
            result = b""
            prev_index = -1
            for index in indexes:
                result += text[prev_index + 1: index]
                prev_index = index
        else:
            result = text
        return eof_byte, result, indexes

    def encode(self, encode, bwt_encoded, bwt_encoded_indices, bwt_encoded_eof):
        eof_byte, result, indexes = self.encode_eof_token(encode)
        with open(encode + ".weof", "wb") as f:
            f.write(result)
        with open(bwt_encoded_indices, "w") as f:
            print(indexes, file=f, end="")
        with open(bwt_encoded_eof, "wb") as f:
            f.write(eof_byte)
        self._call(["encode", encode + ".weof", bwt_encoded, str(int(eof_byte[0]))])

    def decode(self, bwt_encoded, bwt_encoded_indices, bwt_encoded_eof, bwt_decoded):
        with open(bwt_encoded_eof, "rb") as f:
            eof_byte = f.read()
        self._call(["decode", bwt_encoded, bwt_decoded, str(int(eof_byte[0]))])
        with open(bwt_decoded, "rb") as f:
            text = f.read()
        with open(bwt_encoded_indices, "r") as f:
            indexes = []
            for index in f.read().replace("[", "").replace("]", "").split(","):
                if len(index) > 0:
                    indexes.append(int(index))
        self.decode_eof_token(bwt_decoded, eof_byte, text, indexes)

    @staticmethod
    def decode_eof_token(
        output_file_name: str, eof_byte: bytes, text: bytes, indexes: List[int]
    ) -> None:
        if indexes:
            result = b""
            prev_index = -1
            for index in indexes:
                result += text[prev_index + 1: index] + eof_byte
                prev_index = index
        else:
            result = text
        with open(output_file_name, "wb") as f:
            f.write(result)

    def _call(self, args):
        process = subprocess.Popen([self.binary, *args], shell=False)
        errcode = process.wait()
        if errcode != 0:
            out, err = process.communicate()
            print(f"Failed to use bwt. Error code: {errcode}. Args: {args}")
            print(str(out))
            print(str(err))

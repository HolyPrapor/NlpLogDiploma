# fmt: off
from argparse import ArgumentError
import os

import tqdm

from arithmetic_log_model.lru_window import LRUWindow


import pyximport; pyximport.install()
import utils.find_subarray as fs
import filecmp
# fmt: on
import numpy as np
from math import floor, log2, ceil
from typing import *
from iostream.input_stream import BitInputStream
from arithmetic_log_model.trie import MTFNode


def read_byte(stream: BitInputStream):
    cur = 0
    for _ in range(8):
        cur <<= 1
        cur |= stream.read()
    return cur


class AbstractRecordStorage:
    def __init__(self, window_size) -> None:
        self.log_records = []
        self.window_size = window_size

    def store_record(self, record):
        raise NotImplementedError()

    # index in sliding window, start index in record, length
    def get_link(
        self, line, index
    ) -> Tuple[Optional[int], Optional[int], Optional[int]]:
        raise NotImplementedError()

    def move_to_front(self, record_index):
        pass

    def drop(self):
        raise NotImplementedError()


class GreedyAbstractRecordStorage(AbstractRecordStorage):
    def __init__(self, window_size) -> None:
        super().__init__(window_size)

    def get_link(
        self, line, start
    ) -> Tuple[Optional[int], Optional[int], Optional[int]]:
        length = 1
        record_index = record_start_index = record_length = None
        for r, record in enumerate(self.log_records):
            index = fs.find_subarray(record, line[start : start + length])
            while length < len(line) - start and index != -1:
                record_index = r
                record_start_index = index
                record_length = length
                length += 1
                index = fs.find_subarray(record, line[start : start + length])
        return record_index, record_start_index, record_length


class AbstractBaseCoder:
    def __init__(self, is_binary=False) -> None:
        self.is_binary = is_binary

    def encode_link(self, record_index, start_index, length) -> List[int]:
        raise NotImplementedError()

    def decode_link(self, record, i) -> Tuple[int, int, int, int]:
        raise NotImplementedError()

    def encode_int(self, value, size=None) -> List[int]:
        raise NotImplementedError()

    def decode_int(self, record, i, size=None) -> Tuple[int, int]:
        raise NotImplementedError()

    def decode_int_from_stream(self, stream: BitInputStream, size: int) -> int:
        raise NotImplementedError()

    def decode_link_from_stream(self, stream) -> Tuple[int, int, int]:
        raise NotImplementedError()

    def encode_token(self, char) -> List[int]:
        raise NotImplementedError()

    def decode_token(self, record, i) -> Tuple[int, int]:
        raise NotImplementedError()

    def should_decode_as_link(self, record, i) -> bool:
        raise NotImplementedError()

    def should_encode_as_link(
        self, record, start, record_index, start_index, length
    ) -> bool:
        raise NotImplementedError()


class UnaryCoder(AbstractBaseCoder):
    def __init__(self, is_binary=False) -> None:
        super().__init__(True)

    def encode_link(self, record_index, start_index, length) -> List[int]:
        return (
            self.encode_int(record_index)
            + self.encode_int(start_index)
            + self.encode_int(length)
        )

    def encode_int(self, value, size=None) -> List[int]:
        return [1] * value + [0]


class EliasDeltaCoder(AbstractBaseCoder):
    def __init__(self, is_binary=False) -> None:
        super().__init__(True)

    def encode_link(self, record_index, start_index, length) -> List[int]:
        return (
            self.encode_int(record_index)
            + self.encode_int(start_index)
            + self.encode_int(length)
        )

    def encode_int(self, value, size=None) -> List[int]:
        l = 1 + int(floor(log2(value)) if value > 0 else 0)
        ll = int(floor(log2(l)))
        res = []
        for i in range(ll):
            res.append(0)
        for i in range(ll, -1, -1):
            res.append((l >> i) & 1)
        for i in range(l - 2, -1, -1):
            res.append((value >> i) & 1)
        return res
        # return super().encode_int(value, size)


class TripleCoder(AbstractBaseCoder):
    def __init__(self) -> None:
        super().__init__(True)

    def encode_link(self, record_index, start_index, length) -> List[int]:
        return (
            self.encode_int(record_index)
            + self.encode_int(start_index)
            + self.encode_int(length)
        )

    def encode_int(self, value, size=None) -> List[int]:
        tripled = self._triple(value)
        res = []
        if len(tripled):
            tripled[0] -= 1
            res.append(tripled[0])
        else:
            return [1, 1]
        for digit in tripled[1:]:
            if digit == 0:
                res.append(0)
                res.append(0)
            elif digit == 1:
                res.append(0)
                res.append(1)
            else:
                res.append(1)
                res.append(0)
        res.append(1)
        res.append(1)
        return res

    def _triple(self, n: int):
        res = []
        while n > 0:
            res.append(n % 3)
            n //= 3
        res.reverse()
        return res


class SlidingWindowRecordStorage(GreedyAbstractRecordStorage):
    def __init__(self, window_size=100) -> None:
        super().__init__(window_size)

    def store_record(self, record):
        if not len(record):
            return
        self.log_records = [record] + self.log_records
        # self.log_records.append(record)
        if len(self.log_records) > self.window_size:
            self.log_records.pop()
            # self.log_records.pop(0)

    def drop(self):
        self.log_records = []

    def __str__(self) -> str:
        return f"SlidingWindow({self.window_size})"


class SlidingWindowWithDifferentSuffixesRecordStorage(GreedyAbstractRecordStorage):
    def __init__(self, window_size=100, reversed=False) -> None:
        super().__init__(window_size)

    def store_record(self, record):
        if not len(record):
            return
        suffix = record[-10:]
        for line in self.log_records:
            if (line[-10:] == suffix).all():
                return
        self.log_records = [record] + self.log_records
        # self.log_records.append(record)
        if len(self.log_records) > self.window_size:
            self.log_records.pop()
            # self.log_records.pop(0)

    def drop(self):
        self.log_records = []

    def __str__(self) -> str:
        return f"SlidingWindowWithDifferentSuffixes({self.window_size})"


class MoveToFrontStorage(GreedyAbstractRecordStorage):
    def __init__(self, window_size=100, reversed=False) -> None:
        super().__init__(window_size)

    def store_record(self, record):
        if not len(record):
            return
        # suffix = record[-10:]
        # for line in self.log_records:
        #     if (line[-10:] == suffix).all():
        #         return
        self.log_records = [record] + self.log_records
        # self.log_records.append(record)
        if len(self.log_records) > self.window_size:
            self.log_records.pop()
            # self.log_records.pop(0)

    def move_to_front(self, record_index):
        self.log_records = (
            [self.log_records[record_index]]
            + self.log_records[:record_index]
            + self.log_records[record_index + 1 :]
        )

    def drop(self):
        self.log_records = []

    def __str__(self) -> str:
        return f"MoveToFrontStorage({self.window_size})"


class MoveToFrontTrieStorage(AbstractRecordStorage):
    def __init__(self, window_size=30) -> None:
        self.window_size = window_size
        self.root = MTFNode()
        self.index_window = LRUWindow()
        self.log_records = []

    def get_link(
        self, line, start
    ) -> Tuple[Optional[int], Optional[int], Optional[int]]:
        cur = self.root
        i = 0
        while start + i < len(line) and line[start + i] in cur.children:
            cur = cur.children[line[start + i]]
            i += 1
        if i == 0:
            return None, None, None
        # TODO: Track start index somehow
        return min(cur.indices), 0, i

    def store_record(self, record):
        if len(self.log_records) < self.window_size:
            self.index_window.append(len(self.log_records))
            self._add_to_trie(record, len(self.log_records))
            self.log_records.append(record)
        else:
            index = self.index_window.pop()
            self._remove_from_trie(self.log_records[index], index)
            self.index_window.append(index)
            self._add_to_trie(record, index)
            self.log_records[index] = record

    def move_to_front(self, record_index):
        self.index_window.move_to_front(record_index)

    def drop(self):
        self.log_records = []
        self.index_window = LRUWindow()
        self.root = MTFNode()

    def _add_to_trie(self, record, index):
        for i in range(len(record)):
            self.root.add(record, i, index)

    def _remove_from_trie(self, record, index):
        for i in range(len(record)):
            self.root.remove(record, i, index)


class NaiveCoder(AbstractBaseCoder):
    def __init__(self, max_storage_size=100, ignore_super_symbol=False):
        super().__init__(False)
        self.ignore_super_symbol = ignore_super_symbol
        self.super_symbol = "~"
        self.window_size = max_storage_size
        self._byte_mask = (1 << 8) - 1
        self.record_index_size = int(ceil(log2(max_storage_size) / 8))
        self.start_index_size = 2
        self.length_size = 2
        self.link_size = (
            self.record_index_size + self.start_index_size + self.length_size
        )
        if not ignore_super_symbol:
            self.link_size += 1

    def __str__(self) -> str:
        return f"NaiveCoder(IgnoreSuperSymbol: {self.ignore_super_symbol})"

    def encode_link(self, record_index, start_index, length):
        if not self.ignore_super_symbol:
            return (
                self.encode_int(ord(self.super_symbol), 1)
                + self.encode_int(record_index, self.record_index_size)
                + self.encode_int(start_index, self.start_index_size)
                + self.encode_int(length, self.length_size)
            )
        return (
            self.encode_int(record_index, self.record_index_size)
            + self.encode_int(start_index, self.start_index_size)
            + self.encode_int(length, self.length_size)
        )

    def decode_link(self, record, i):
        assert len(record) - i >= self.link_size
        decoded = []
        cur = i + 1
        for size in [self.record_index_size, self.start_index_size, self.length_size]:
            decoded.append(self.decode_int(record, cur, size)[0])
            cur += size
        return decoded[0], decoded[1], decoded[2], cur

    def encode_int(self, value, size=None) -> List[int]:
        encoded = []
        cur = value
        for _ in range(size):
            encoded.append(cur & self._byte_mask)
            cur >>= 8
        return encoded

    def decode_int(self, record, i, size=1):
        cur = 0
        for char in reversed(record[i : i + size]):
            cur <<= 8
            cur |= char
        return cur, i + size

    def decode_int_from_stream(self, stream: BitInputStream, size: int) -> int:
        return self.decode_int([read_byte(stream) for _ in range(size)], 0, size)[0]

    def decode_link_from_stream(self, stream) -> Tuple[int, int, int]:
        buffer = [read_byte(stream) for _ in range(self.link_size)]
        record_index, start_index, length, _ = self.decode_link(buffer, 0)
        return record_index, start_index, length

    def encode_token(self, token) -> List[int]:
        return [token]

    def decode_token(self, record, i) -> Tuple[int, int]:
        return record[i], i + 1

    def should_decode_as_link(self, record, i) -> bool:
        return chr(record[i]) == self.super_symbol

    def should_encode_as_link(
        self, record, start, record_index, start_index, length
    ) -> bool:
        return (
            length > self.link_size
            and record_index is not None
            and start_index is not None
        )


class SmartCoder(AbstractBaseCoder):
    def __init__(self, max_value: int = 127):
        super().__init__(False)
        if max_value <= 0 or max_value > 255:
            raise ArgumentError(f"{max_value} is not supported")
        self.max_value = max_value

    def __str__(self) -> str:
        return f"SmartCoder({self.max_value})"

    def encode_int(self, value, size=None) -> List[int]:
        encoded = []
        while value >= self.max_value:
            encoded.append(255)
            value -= self.max_value
        encoded.append(255 - self.max_value + value)
        return encoded

    def decode_int(self, record, i, size=None) -> Tuple[int, int]:
        value = 0
        while record[i] == 255:
            value += self.max_value
            i += 1
        value += record[i] - (255 - self.max_value)
        i += 1
        return value, i

    def decode_int_from_stream(self, stream, size=None) -> int:
        decoded = [read_byte(stream)]
        while decoded[-1] == 255:
            decoded.append(read_byte(stream))
        return self.decode_int(decoded, 0)[0]

    def decode_link_from_stream(self, stream) -> Tuple[int, int, int]:
        return [self.decode_int_from_stream(stream) for _ in range(3)]

    def encode_token(self, token) -> List[int]:
        if token < self.max_value:
            return [token]
        return [self.max_value, token]

    def decode_token(self, record, i) -> Tuple[int, int]:
        if (
            i + 1 < len(record)
            and record[i] == self.max_value
            and record[i + 1] >= self.max_value
        ):
            return record[i + 1], i + 2
        return record[i], i + 1

    def should_decode_as_link(self, record, i) -> bool:
        return record[i] >= 255 - self.max_value

    def _count_space_for_int_encoding(self, value):
        return value // self.max_value + 1

    def should_encode_as_link(
        self, record, start, record_index, start_index, length
    ) -> bool:
        return (
            record_index is not None
            and start_index is not None
            and self._count_space_for_int_encoding(record_index)
            + self._count_space_for_int_encoding(start_index)
            + self._count_space_for_int_encoding(length)
            < length
        )

    def encode_link(self, record_index, start_index, length) -> List[int]:
        return (
            self.encode_int(record_index)
            + self.encode_int(start_index)
            + self.encode_int(length)
        )

    def decode_link(self, record, i) -> Tuple[int, int, int, int]:
        record_index, i = self.decode_int(record, i)
        start_index, i = self.decode_int(record, i)
        length, i = self.decode_int(record, i)
        return record_index, start_index, length, i


class Compressor:
    def __init__(self, coder, storage):
        self.coder = coder
        self.storage = storage

    def compress(self, line: List[int]) -> List[int]:
        res = []
        start = 0
        while start < len(line):
            record_index, record_start_index, record_length = self.storage.get_link(
                line, start
            )
            record_length = record_length or 1
            if self.coder.should_encode_as_link(
                line, start, record_index, record_start_index, record_length
            ):
                res += self.coder.encode_link(
                    record_index, record_start_index, record_length
                )
                self.storage.move_to_front(record_index)
            else:
                for i in range(record_length):
                    res += self.coder.encode_token(line[start + i])
            start += record_length
        self.storage.store_record(line)
        return res

    def compress_lines(self, lines: List[List[int]]) -> List[int]:
        compressed = []
        i = 0
        for line in tqdm.tqdm(lines, desc="Log compressing"):
            i += 1
            encoded = self.compress(line)
            compressed += self.coder.encode_int(len(encoded), 2) + encoded
        return bytes(compressed)


class Decompressor:
    def __init__(self, coder, storage):
        self.coder = coder
        self.storage = storage

    def decompress(self, line: List[int]) -> List[int]:
        i = 0
        cur = []
        while i < len(line):
            if self.coder.should_decode_as_link(line, i):
                record_index, start_index, length, i = self.coder.decode_link(line, i)
                parsed_link = self.storage.log_records[record_index][
                    start_index : start_index + length
                ]
                cur += parsed_link
            else:
                decoded_char, i = self.coder.decode_token(line, i)
                cur += [decoded_char]
        self.storage.store_record(cur)
        return cur

    def decompress_lines(self, lines: List[int]) -> List[List[int]]:
        decompressed = []
        text = lines
        i = 0
        while i < len(text):
            cur_length, i = self.coder.decode_int(text, i, 2)
            decompressed.append(self.decompress(text[i : i + cur_length]))
            i += cur_length
        return decompressed


def compress(compressor, text):
    # crop line endings
    lines = [
        np.array([ord(ch) for ch in line[:-1] if ord(ch) < 256], dtype=np.int32)
        for line in text
    ]
    return compressor.compress_lines(lines)


def decompress(decompressor, bytetext):
    decompressed = decompressor.decompress_lines(bytetext)
    # put back line endings
    return "\n".join(["".join([chr(byte) for byte in line]) for line in decompressed])


def encode(
    input_file: str,
    output_file_prefix: str,
    coder: AbstractBaseCoder = None,
    storage: AbstractRecordStorage = None,
):
    if coder is None:
        coder = SmartCoder()
    if storage is None:
        storage = SlidingWindowRecordStorage()

    compressor = Compressor(coder, storage)
    with open(input_file, mode="r") as f:
        text = f.readlines()
    with open(output_file_prefix + "_final", mode="wb") as f:
        f.write(compress(compressor, text))


def decode(
    encoded_file_prefix: str,
    decoded_file: str,
    coder: AbstractBaseCoder = None,
    storage: AbstractRecordStorage = None,
):
    if coder is None:
        coder = SmartCoder()
    if storage is None:
        storage = SlidingWindowRecordStorage()

    decompressor = Decompressor(coder, storage)
    with open(encoded_file_prefix + "_final", mode="rb") as f:
        bytetext = f.read()
    with open(decoded_file, mode="w") as f:
        print(decompress(decompressor, bytetext), file=f)


if __name__ == "__main__":
    import sys

    sys.setrecursionlimit(5000)
    # encode("test_files/logs/small/android.log", "out")
    # decode("out", "decoded")
    # if not filecmp.cmp("test_files/logs/small/android.log", "decoded", False):
    #     print("NO")
    for file in ["windows"]:
        encode(
            f"test_files/logs/small/{file}.log",
            f"out_{file}",
            NaiveCoder(30),
            MoveToFrontStorage(30),
        )
        # decode(f"out_{file}", f"decoded_{file}.txt")
        # if not filecmp.cmp(
        #     f"test_files/logs/small/{file}.log", f"decoded_{file}.txt", False
        # ):
        #     print(f"test_files/logs/small/{file}.log", f"decoded_{file}.txt")

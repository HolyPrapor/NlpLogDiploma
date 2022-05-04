import numpy as np
from math import log2, ceil
from typing import *
import utils.find_subarray as fs
import pyximport
pyximport.install()


class AbstractRecordStorage:
    def __init__(self) -> None:
        self.log_records = []

    def store_record(self, record):
        raise NotImplementedError()

    def drop(self):
        raise NotImplementedError()


class AbstractBaseCoder:
    def encode_link(self, record_index, start_index, length) -> List[int]:
        raise NotImplementedError()

    def decode_link(self, record, i) -> Tuple[int, int, int, int]:
        raise NotImplementedError()

    def encode_int(self, value, size=None) -> List[int]:
        raise NotImplementedError()

    def decode_int(self, record, i, size=None) -> Tuple[int, int]:
        raise NotImplementedError()

    def encode_token(self, char) -> List[int]:
        raise NotImplementedError()

    def decode_token(self, record, i) -> Tuple[int, int]:
        raise NotImplementedError()

    def should_decode_as_link(self, record, i) -> bool:
        raise NotImplementedError()

    def should_encode_as_link(self, record, start, record_index, start_index, length) -> bool:
        raise NotImplementedError()


class SlidingWindowRecordStorage(AbstractRecordStorage):
    def __init__(self, window_size=100) -> None:
        super().__init__()
        self.window_size = window_size

    def store_record(self, record):
        self.log_records.append(record)
        if len(self.log_records) > self.window_size:
            self.log_records.pop(0)

    def drop(self):
        self.log_records = []


class NaiveCoder(AbstractBaseCoder):
    def __init__(self, max_storage_size):
        self.super_symbol = '~'
        self.window_size = max_storage_size
        self._byte_mask = (1 << 8) - 1
        self.record_index_size = int(ceil(log2(max_storage_size) / 8))
        self.start_index_size = 2
        self.length_size = 2
        self.link_size = (1 +
                          self.record_index_size +
                          self.start_index_size +
                          self.length_size)

    def encode_link(self, record_index, start_index, length):
        return (self.encode_int(ord(self.super_symbol), 1) +
                self.encode_int(record_index, self.record_index_size) +
                self.encode_int(start_index, self.start_index_size) +
                self.encode_int(length, self.length_size))

    def decode_link(self, record, i):
        assert(len(record) - i >= self.link_size)
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

    def decode_int(self, record, i, size=None):
        cur = 0
        for char in reversed(record[i:i+size]):
            cur <<= 8
            cur |= char
        return cur, i + size

    def encode_token(self, token) -> List[int]:
        return [token]

    def decode_token(self, record, i) -> Tuple[int, int]:
        return record[i], i + 1

    def should_decode_as_link(self, record, i) -> bool:
        return chr(record[i]) == self.super_symbol

    def should_encode_as_link(self, record, start, record_index, start_index, length) -> bool:
        return length > self.link_size and record_index is not None and start_index is not None


class SmartCoder(AbstractBaseCoder):
    def __init__(self):
        pass

    def encode_int(self, value, size=None) -> List[int]:
        encoded = []
        while value >= 127:
            encoded.append(255)
            value -= 127
        encoded.append(128 + value)
        return encoded

    def decode_int(self, record, i, size=None) -> Tuple[int, int]:
        value = 0
        while record[i] == 255:
            value += 127
            i += 1
        value += record[i] - 128
        i += 1
        return value, i

    def encode_token(self, token) -> List[int]:
        if token < 127:
            return [token]
        return [127, token]

    def decode_token(self, record, i) -> Tuple[int, int]:
        if i + 1 < len(record) and record[i] == 127 and record[i + 1] >= 127:
            return record[i + 1], i + 2
        return record[i], i + 1

    def should_decode_as_link(self, record, i) -> bool:
        return record[i] >= 128

    def _count_space_for_int_encoding(self, value):
        return value // 127 + 1

    def should_encode_as_link(self, record, start, record_index, start_index, length) -> bool:
        return record_index is not None and start_index is not None and self._count_space_for_int_encoding(record_index) + self._count_space_for_int_encoding(start_index) + self._count_space_for_int_encoding(length) < length

    def encode_link(self, record_index, start_index, length) -> List[int]:
        return self.encode_int(record_index) + self.encode_int(start_index) + self.encode_int(length)

    def decode_link(self, record, i) -> Tuple[int, int, int, int]:
        record_index, i = self.decode_int(record, i)
        start_index, i = self.decode_int(record, i)
        length, i = self.decode_int(record, i)
        return record_index, start_index, length, i


class Compressor:
    def __init__(self, coder, storage):
        self.coder = coder
        self.storage = storage

#     def compress(self, line: List[int]) -> List[int]:
#         res = []
#         start = 0
#         while start < len(line):
#             length = 1
#             record_index = record_start_index = None
#             for r, record in enumerate(self.storage.log_records):
#                 while length < len(line) - start and (index := fs.find_subarray(record, line[start:start + length])) != -1:
#                     record_index = r
#                     record_start_index = index
#                     length += 1
#             if self.coder.should_encode_as_link(line, start, record_index, record_start_index, length):
#                 length -= 1
#                 res += self.coder.encode_link(record_index,
#                                               record_start_index, length)
#             else:
#                 for i in range(length):
#                     res += self.coder.encode_token(line[start + i])
#             start += length
#         self.storage.store_record(line)
#         return res

    def compress_lines(self, lines: List[List[int]]) -> List[int]:
        compressed = []
        i = 0
        for line in lines:
            # print(f"{i}/{len(lines)}")
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
                record_index, start_index, length, i = self.coder.decode_link(
                    line, i)
                parsed_link = self.storage.log_records[record_index][start_index:start_index + length]
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
            decompressed.append(self.decompress(text[i:i+cur_length]))
            i += cur_length
        return decompressed


def compress(compressor, text):
    # crop line endings
    lines = [np.array([ord(ch) for ch in line[:-1]], dtype=np.int32)
             for line in text]
    return compressor.compress_lines(lines)


def decompress(decompressor, bytetext):
    decompressed = decompressor.decompress_lines(bytetext)
    # put back line endings
    return '\n'.join([''.join([chr(byte) for byte in line]) for line in decompressed])


if __name__ == '__main__':
    window_size = 20
    coder = SmartCoder()
    storage = SlidingWindowRecordStorage(window_size)

    compressor = Compressor(coder, storage)
    with open('log_model/in.txt', mode='r') as f:
        text = f.readlines()
    with open('log_model/out.txt', mode='wb') as f:
        f.write(compress(compressor, text))

    storage.drop()

    decompressor = Decompressor(coder, storage)
    with open('log_model/out.txt', mode='rb') as f:
        bytetext = f.read()
    with open('log_model/decoded.txt', mode='w') as f:
        print(decompress(decompressor, bytetext), file=f)

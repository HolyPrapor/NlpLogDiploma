from math import log2, ceil
from typing import *


class AbstractRecordStorage:
    def __init__(self) -> None:
        self.log_records = []

    def store_record(self, record):
        raise NotImplementedError()


class AbstractBaseCoder:
    def encode_link(self, record_index, start_index, length) -> List[int]:
        raise NotImplementedError()

    def decode_link(self, record, i) -> Tuple[int, int, int, int]:
        raise NotImplementedError()

    def encode_int(self, value, size) -> List[int]:
        raise NotImplementedError()

    def decode_int(self, record, i, size) -> Tuple[int, int]:
        raise NotImplementedError()

    def encode_char(self, char) -> List[int]:
        raise NotImplementedError()

    def decode_char(self, record, i) -> Tuple[int, int]:
        raise NotImplementedError()

    def should_decode_as_link(self, record, i) -> bool:
        raise NotImplementedError()

    def should_encode_as_link(self, record, start, end) -> bool:
        raise NotImplementedError()


class SlidingWindowRecordStorage(AbstractRecordStorage):
    def __init__(self, window_size=100) -> None:
        super().__init__()
        self.window_size = window_size

    def store_record(self, record):
        self.log_records.append(record)
        if len(self.log_records) > self.window_size:
            self.log_records.pop(0)


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
        return *decoded, cur

    def encode_int(self, value, size) -> List[int]:
        encoded = []
        cur = value
        for _ in range(size):
            encoded.append(cur & self._byte_mask)
            cur >>= 8
        return encoded

    def decode_int(self, record, i, size):
        cur = 0
        for char in reversed(record[i:i+size]):
            cur <<= 8
            cur |= char
        return cur, i + size

    def encode_char(self, char) -> List[int]:
        return [ord(char)]

    def decode_char(self, record, i) -> Tuple[int, int]:
        return chr(record[i]), i + 1

    def should_decode_as_link(self, record, i) -> bool:
        return chr(record[i]) == self.super_symbol

    def should_encode_as_link(self, record, start, end) -> bool:
        return end - start + 1 > self.link_size


class Compressor:
    def __init__(self, window_size=100):
        self.coder = NaiveCoder(window_size)
        self.storage = SlidingWindowRecordStorage(window_size)

    def compress(self, line: str) -> str:
        res = []
        start = 0
        while start < len(line):
            length = 1
            record_index = record_start_index = None
            for r, record in enumerate(self.storage.log_records):
                while length < len(line) - start and (index := record.find(line[start:start + length])) != -1:
                    record_index = r
                    record_start_index = index
                    length += 1
            if self.coder.should_encode_as_link(line, start, start + length) and record_index is not None and record_start_index is not None:
                length -= 1
                res += self.coder.encode_link(record_index,
                                              record_start_index, length)
            else:
                for i in range(length):
                    res += self.coder.encode_char(line[start + i])
            start += length
        self.storage.store_record(line)
        return res

    def compress_lines(self, lines):
        compressed = []
        for line in lines:
            encoded = self.compress(line[:-1])  # remove eol tokens
            compressed += self.coder.encode_int(len(encoded) + 2, 2) + encoded
        return bytes(compressed)


class Decompressor:
    def __init__(self, window_size=100):
        self.coder = NaiveCoder(window_size)
        self.storage = SlidingWindowRecordStorage(window_size)

    def decompress(self, line: str) -> str:
        i = 0
        cur = []
        while i < len(line):
            if self.coder.should_decode_as_link(line, i):
                record_index, start_index, length, i = self.coder.decode_link(
                    line, i)
                parsed_link = self.storage.log_records[record_index][start_index:start_index + length]
                cur.append(parsed_link)
            else:
                decoded_char, i = self.coder.decode_char(line, i)
                cur.append(decoded_char)
        decoded = ''.join(cur)
        self.storage.store_record(decoded)
        return decoded

    def decompress_lines(self, lines):
        decompressed = []
        text = lines
        i = 0
        while i < len(text):
            cur_length, _ = self.coder.decode_int(text, i, 2)
            decompressed.append(self.decompress(text[i+2:i+cur_length]))
            i += cur_length
        return '\n'.join(decompressed)


window_size = 150

compressor = Compressor(window_size)
with open('in.txt', mode='r') as f:
    text = f.readlines()
with open('out.txt', mode='wb') as f:
    f.write(compressor.compress_lines(text))

decompressor = Decompressor(window_size)
with open('out.txt', mode='rb') as f:
    text = f.read()
with open('decoded.txt', mode='w') as f:
    print(decompressor.decompress_lines(text), file=f)

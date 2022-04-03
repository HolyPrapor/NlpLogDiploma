class BaseCoder:
    def __init__(self, window_size):
        self.link_size = len(self._encode_link(0, 0, 0))
        self.super_symbol = '~'
        self.window_size = window_size
        self.log_records = []

    def _store_record(self, record):
        self.log_records.append(record)
        if len(self.log_records) > self.window_size:
            self.log_records.pop(0)

    def _encode_link(self, record_index, start_index, length):
        return self._to_bytes(ord(self.super_symbol), 1) + self._to_bytes(record_index, 1) + self._to_bytes(start_index, 2) + self._to_bytes(length, 2)

    def _decode_link(self, record, i):
        assert(len(record) - i >= self.link_size)
        record_index = self._from_bytes(record[i+1:i+2])
        start_index = self._from_bytes(record[i+2:i+4])
        length = self._from_bytes(record[i+4:i+6])
        return record_index, start_index, length

    def _to_bytes(self, value, size):
        encoded = []
        cur = value
        mask = (1 << 8) - 1
        for _ in range(size):
            encoded.append(cur & mask)
            cur >>= 8
        return encoded

    def _from_bytes(self, encoded):
        cur = 0
        for char in reversed(encoded):
            cur <<= 8
            cur |= char
        return cur


class Compressor(BaseCoder):
    def __init__(self, window_size=100):
        super().__init__(window_size)

    def compress(self, line: str) -> str:
        res = []
        start = 0
        while start < len(line):
            length = 1
            record_index = record_start_index = None
            for r, record in enumerate(self.log_records):
                while length < len(line) - start and (index := record.find(line[start:start + length])) != -1:
                    record_index = r
                    record_start_index = index
                    length += 1
            if length > self.link_size and record_index is not None and record_start_index is not None:
                length -= 1
                res += self._encode_link(record_index,
                                         record_start_index, length)
            else:
                for i in range(length):
                    res.append(ord(line[start + i]))
            start += length
        self._store_record(line)
        return res

    def compress_lines(self, lines):
        compressed = []
        for line in lines:
            encoded = self.compress(line[:-1])  # remove eol tokens
            compressed += self._to_bytes(len(encoded) + 2, 2) + encoded
        return bytes(compressed)


class Decompressor(BaseCoder):
    def __init__(self, window_size=100):
        super().__init__(window_size)

    def decompress(self, line: str) -> str:
        i = 0
        cur = []
        while i < len(line):
            if chr(line[i]) == self.super_symbol:
                record_index, start_index, length = self._decode_link(line, i)
                parsed_link = self.log_records[record_index][start_index:start_index + length]
                cur.append(parsed_link)
                i += self.link_size
            else:
                cur.append(chr(line[i]))
                i += 1
        decoded = ''.join(cur)
        self._store_record(decoded)
        return decoded

    def decompress_lines(self, lines):
        decompressed = []
        text = lines
        i = 0
        while i < len(text):
            cur_length = self._from_bytes(text[i:i+2])
            decompressed.append(self.decompress(text[i+2:i+cur_length]))
            i += cur_length
        return '\n'.join(decompressed)


window_size = 255

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

class Compressor:
    def __init__(self, window_size=50):
        self.prev_log_records = []
        self.window_size = window_size
        self.super_symbol = '~'

    def compress(self, line: str) -> str:
        res = []
        start = end = 0
        new_line = line
        while new_line:
            new_line = line[start:]
            i = 1
            new_end = end
            start_index = None
            index = None
            for j, record in enumerate(self.prev_log_records):
                while x := record.find(new_line[:i]) != -1 and i < len(new_line):
                    new_end = end + i
                    i += 1
                    index = j
                    start_index = x
            end = new_end
            if end - start > 6:
                link = self.super_symbol + chr(index) + chr(start_index >> 8) + chr(start_index & (2 ** 8 - 1)) + chr(
                    (end - start) >> 8) + chr((end - start) & (2 ** 8 - 1))
                res.append(link)
                start = end
            else:
                if end - start > 1:
                    res.append(line[start:end])
                    start = end
                else:
                    res.append(line[start])
                    start += 1
                    end += 1
            if start == len(line):
                break
        if len(self.prev_log_records) >= self.window_size:
            self.prev_log_records.pop(0)
        self.prev_log_records.append(line)
        return ''.join(res)


class Decompressor:
    def __init__(self, window_size=50):
        self.prev_log_records = []
        self.window_size = window_size

    def decompress(self, line: str) -> str:
        if not self.prev_log_records:
            self.prev_log_records.append(line)
            return line
        res = []
        # TODO: add decompress
        self.prev_log_records.append(''.join(res))
        return ''.join(res)


compressor = Compressor(2000)
with open('in.txt', mode='r') as f:
    text = f.readlines()
with open('out.txt', mode='w') as f:
    print(''.join([compressor.compress(line) for line in text]), file=f)

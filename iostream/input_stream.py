from typing import BinaryIO


class BitInputStream:
    def __init__(self, input_stream: BinaryIO):
        self.input_stream = input_stream
        self.current_byte = 0
        self.num_bits_remaining = 0

    # TODO: add bit abstraction
    def read(self):
        if self.current_byte == -1:
            return -1
        if self.num_bits_remaining == 0:
            temp = self.input_stream.read(1)
            if len(temp) == 0:
                self.current_byte = -1
                return -1
            self.current_byte = temp[0]
            self.num_bits_remaining = 8
        assert self.num_bits_remaining > 0
        self.num_bits_remaining -= 1
        return (self.current_byte >> self.num_bits_remaining) & 1

    # TODO: add bit abstraction
    def read_no_eof(self):
        result = self.read()
        if result != -1:
            return result
        else:
            raise EOFError()

    def close(self) -> None:
        self.input_stream.close()
        self.current_byte = -1
        self.num_bits_remaining = 0

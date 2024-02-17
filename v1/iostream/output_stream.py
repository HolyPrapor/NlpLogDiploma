from typing import BinaryIO


class BitOutputStream:
    def __init__(self, output_stream: BinaryIO):
        self.output_stream = output_stream
        self.current_byte = 0
        self.num_bits_filled = 0

    # TODO: add bit abstraction
    def write(self, bit) -> None:
        assert bit in (0, 1)
        self.current_byte = (self.current_byte << 1) | bit
        self.num_bits_filled += 1
        if self.num_bits_filled == 8:
            self.output_stream.write(bytes((self.current_byte,)))
            self.current_byte = 0
            self.num_bits_filled = 0

    def close(self) -> None:
        while self.num_bits_filled != 0:
            self.write(0)
        self.output_stream.close()

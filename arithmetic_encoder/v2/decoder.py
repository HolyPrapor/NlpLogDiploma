from typing import List

from arithmetic_encoder.v2.base_coder import ArithmeticCoderBase
from iostream.input_stream import BitInputStream


class ArithmeticDecoder(ArithmeticCoderBase):
    def __init__(self, num_bits: int, input_stream: BitInputStream):
        super(ArithmeticDecoder, self).__init__(num_bits)
        self.input_stream = input_stream
        self.code = 0
        for _ in range(self.num_state_bits):
            self.code = self.code << 1 | self.read_code_bit()

    def read(self, frequencies: List[int]) -> int:
        total = frequencies[-1]

        range_length = self.high - self.low + 1
        offset = self.code - self.low
        value = ((offset + 1) * total - 1) // range_length

        start = 0
        end = len(frequencies)
        while end - start > 1:
            middle = (start + end) >> 1
            low = frequencies[middle - 1] if middle > 0 else 0
            if low > value:
                end = middle
            else:
                start = middle

        self.update(frequencies, start)
        return start

    def shift(self) -> None:
        self.code = ((self.code << 1) & self.state_mask) | self.read_code_bit()

    def underflow(self) -> None:
        self.code = (self.code & self.half_range) | ((self.code << 1) & (self.state_mask >> 1)) | self.read_code_bit()

    # TODO: think about bit abstraction
    def read_code_bit(self):
        temp = self.input_stream.read()
        return 0 if temp == -1 else temp

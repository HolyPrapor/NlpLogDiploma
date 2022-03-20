from encoder.v2.base_coder import ArithmeticCoderBase
from iostream.input_stream import BitInputStream
from typing import Dict
from model.model import Token


class ArithmeticDecoder(ArithmeticCoderBase):
    def __init__(self, num_bits: int, input_stream: BitInputStream):
        super(ArithmeticDecoder, self).__init__(num_bits)
        self.input_stream = input_stream
        self.code = 0
        for _ in range(self.num_state_bits):
            self.code = self.code << 1 | self.read_code_bit()

    # TODO: replace int result by token
    def read(self, frequencies: Dict[Token, int]) -> Token:
        total = max(frequencies.values())

        range_length = self.high - self.low + 1
        offset = self.code - self.low
        value = ((offset + 1) * total - 1) // range_length

        start = 0
        end = len(frequencies) + 1
        while end - start > 1:
            middle = (start + end) >> 1
            low = int(frequencies[Token(middle - 1)]) if middle > 1 else 0
            if low > value:
                end = middle
            else:
                start = middle

        symbol = Token(start)
        self.update(frequencies, symbol)
        return symbol

    def shift(self) -> None:
        self.code = ((self.code << 1) & self.state_mask) | self.read_code_bit()

    def underflow(self) -> None:
        self.code = (self.code & self.half_range) | ((self.code << 1) & (self.state_mask >> 1)) | self.read_code_bit()

    # TODO: think about bit abstraction
    def read_code_bit(self):
        temp = self.input_stream.read()
        return 0 if temp == -1 else temp

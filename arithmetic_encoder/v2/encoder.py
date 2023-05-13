from arithmetic_encoder.v2.base_coder import ArithmeticCoderBase
from iostream.output_stream import BitOutputStream
from typing import List


class ArithmeticEncoder(ArithmeticCoderBase):
    def __init__(self, num_bits: int, output_stream: BitOutputStream):
        super(ArithmeticEncoder, self).__init__(num_bits)
        self.output_stream = output_stream
        self.num_underflow = 0

    def write(self, frequencies: List[int], symbol: int) -> None:
        self.update(frequencies, symbol)

    def finish(self) -> None:
        self.output_stream.write(1)

    def shift(self) -> None:
        bit = self.low >> (self.num_state_bits - 1)
        self.output_stream.write(bit)

        for _ in range(self.num_underflow):
            self.output_stream.write(bit ^ 1)
        self.num_underflow = 0

    def underflow(self) -> None:
        self.num_underflow += 1

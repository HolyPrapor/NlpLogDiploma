from typing import List


class ArithmeticCoderBase:
    def __init__(self, num_bits: int = 32):
        if num_bits < 1:
            raise ValueError("State size out of range")

        self.num_state_bits = num_bits
        self.full_range = 1 << self.num_state_bits
        self.half_range = self.full_range >> 1
        self.quarter_range = self.half_range >> 1
        self.minimum_range = self.quarter_range + 2
        self.maximum_total = self.minimum_range
        self.state_mask = self.full_range - 1

        self.low = 0
        self.high = self.state_mask

    def update(self, frequencies: List[int], symbol: int) -> None:
        range_length = self.high - self.low + 1

        total = frequencies[-1]
        symbol_low = frequencies[symbol - 1] if symbol > 0 else 0
        symbol_high = frequencies[symbol]
        low = self.low

        self.low = low + symbol_low * range_length // total
        self.high = low + (symbol_high * range_length // total) - 1

        while ((self.low ^ self.high) & self.half_range) == 0:
            self.shift()
            self.low = ((self.low << 1) & self.state_mask)
            self.high = ((self.high << 1) & self.state_mask) | 1

        while (self.low & ~self.high & self.quarter_range) != 0:
            self.underflow()
            self.low = (self.low << 1) ^ self.half_range
            self.high = ((self.high ^ self.half_range) << 1) | self.half_range | 1

    def shift(self) -> None:
        raise NotImplementedError()

    def underflow(self) -> None:
        raise NotImplementedError()

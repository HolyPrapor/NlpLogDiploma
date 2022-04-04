import utils.find_subarray as fs
from typing import *
from arithmetic_encoder.v2.encoder import ArithmeticEncoder
from probability_model.model import ModelInterface
from iostream.output_stream import BitOutputStream
from log_model.log_compresser import AbstractRecordStorage, AbstractBaseCoder
from main import probabilities_to_frequencies
import pyximport
pyximport.install()


def bitfield(n: int) -> List[int]:
    return [int(digit) for digit in bin(n)[2:]]


class ArithmeticLogEncoder:
    def __init__(self, arithmetic_encoder: ArithmeticEncoder,
                 probability_model: ModelInterface,
                 log_encoder: AbstractBaseCoder,
                 storage: AbstractRecordStorage,
                 output_stream: BitOutputStream) -> None:
        self.arithmetic_encoder = arithmetic_encoder
        self.probability_model = probability_model
        self.log_encoder = log_encoder
        self.storage = storage
        self.output_stream = output_stream

    def _write_to_stream(self, elements: List[int]) -> None:
        for element in elements:
            for bit in bitfield(element):
                self.output_stream.write(bit)

    def encode(self, line: List[int]) -> None:
        start = 0
        while start < len(line):
            length = 1
            record_index = record_start_index = None
            for r, record in enumerate(self.storage.log_records):
                while length < len(line) - start and (index := fs.find_subarray(record, line[start:start + length])) != -1:
                    record_index = r
                    record_start_index = index
                    length += 1
            if length > 6:
                self.output_stream.write(0)
                length -= 1
                encoded = self.log_encoder.encode_link(record_index,
                                                       record_start_index, length)
                self._write_to_stream(encoded)
                self.probability_model.feed(line[:start + length])
            else:
                self.output_stream.write(1)
                probs = self.probability_model.get_probabilities()
                frequencies, _, _ = probabilities_to_frequencies(probs, 32)
                self.arithmetic_encoder.write(frequencies, line[start])
            start += length
            if start < len(line):
                self.probability_model.feed(line[start])
        self.storage.store_record(line)

import numpy as np
from probability_model.lstm_log_model.lstm_log_model import LstmLogModel
from typing import *
from arithmetic_encoder.v2.encoder import ArithmeticEncoder
from arithmetic_encoder.v2.decoder import ArithmeticDecoder
from probability_model.model import ModelInterface, Token
from iostream.output_stream import BitOutputStream
from iostream.input_stream import BitInputStream
from log_model.log_compresser import (
    AbstractRecordStorage,
    AbstractBaseCoder,
    SlidingWindowRecordStorage,
    SmartCoder,
)
from main import probabilities_to_frequencies

# fmt: off
import pyximport; pyximport.install()
import utils.find_subarray as fs
# fmt: on


def bitfield(n: int) -> List[int]:
    return [int((n & (1 << (7 - i))) == (1 << (7 - i))) for i in range(8)]


def get_bits(elements: List[int]) -> List[int]:
    cur = []
    for element in elements:
        cur += bitfield(element)
    return cur


class ArithmeticLogEncoder:
    def __init__(
        self,
        arithmetic_encoder: ArithmeticEncoder,
        probability_model: ModelInterface,
        log_encoder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        lz_output_stream: BitOutputStream,
        auxillary_output_stream: BitOutputStream,
    ) -> None:
        self.arithmetic_encoder = arithmetic_encoder
        self.probability_model = probability_model
        self.log_encoder = log_encoder
        self.storage = storage
        self.lz_output_stream = lz_output_stream
        self.auxillary_output_stream = auxillary_output_stream

    def _write_to_stream(self, elements: List[int], stream: BitOutputStream) -> None:
        for bit in get_bits(elements):
            stream.write(bit)

    def encode_text(self, lines: List[str]) -> None:
        self._write_to_stream(
            self.log_encoder.encode_int(len(lines)), self.auxillary_output_stream
        )
        for i, line in enumerate(lines):
            print("iteration", i)
            tokens = np.array(
                [token.value for token in self.probability_model.preprocess(line)],
                dtype=np.int32,
            )
            operations = self.encode(tokens)
            self._write_to_stream(
                self.log_encoder.encode_int(len(operations)),
                self.auxillary_output_stream,
            )
            for bit in operations:
                self.auxillary_output_stream.write(bit)
        self.arithmetic_encoder.finish()

    def encode(self, line: List[int]) -> List[int]:
        operations = [0] * len(line)
        iterations = 0
        start = 0
        while start < len(line):
            length = 1
            record_index = record_start_index = record_length = None
            for r, record in enumerate(self.storage.log_records):
                index = fs.find_subarray(record, line[start : start + length])
                while length < len(line) - start and index != -1:
                    record_index = r
                    record_start_index = index
                    record_length = length
                    length += 1
                    index = fs.find_subarray(record, line[start : start + length])
            if record_length is not None and record_length > 6:
                length = record_length
                operations[iterations] = 0
                encoded = self.log_encoder.encode_link(
                    record_index, record_start_index, length
                )
                self._write_to_stream(encoded, self.lz_output_stream)
                #                 print("feeding", line[:start + length])
                self.probability_model.feed(line[start : start + length])
            else:
                operations[iterations] = 1
                probs = self.probability_model.get_probabilities()
                frequencies, _, _ = probabilities_to_frequencies(probs, 32)
                self.arithmetic_encoder.write(frequencies, line[start])
                self.probability_model.feed([line[start]])
                length = 1
            start += length
            iterations += 1
        self.storage.store_record(line)
        return operations[:iterations]


class ArithmeticLogDecoder:
    def __init__(
        self,
        arithmetic_decoder: ArithmeticDecoder,
        probability_model: ModelInterface,
        log_encoder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        lz_input_stream: BitInputStream,
        auxillary_input_stream: BitInputStream,
    ) -> None:
        self.arithmetic_decoder = arithmetic_decoder
        self.probability_model = probability_model
        self.log_encoder = log_encoder
        self.storage = storage
        self.lz_input_stream = lz_input_stream
        self.auxillary_input_stream = auxillary_input_stream

    def _read_byte(self, stream: BitInputStream):
        cur = 0
        for _ in range(8):
            cur <<= 1
            cur |= stream.read()
        return cur

    def _get_next_int(self, stream: BitInputStream):
        decoded = [self._read_byte(stream)]
        while decoded[-1] == 255:
            decoded.append(self._read_byte(stream))
        return self.log_encoder.decode_int(decoded, 0)[0]

    def decode_text(self) -> List[str]:
        res = []
        length = self._get_next_int(self.auxillary_input_stream)
        for i in range(length):
            tokens = self.decode()
            res.append(
                self.probability_model.postprocess([Token(value) for value in tokens])
            )
        self.arithmetic_decoder
        return res

    def decode(self) -> List[int]:
        iterations = self._get_next_int(self.auxillary_input_stream)
        line = []
        while iterations > 0:
            mode = self.auxillary_input_stream.read()
            if mode == 0:
                record_index = self._get_next_int(self.lz_input_stream)
                start_index = self._get_next_int(self.lz_input_stream)
                length = self._get_next_int(self.lz_input_stream)
                parsed_link = self.storage.log_records[record_index][
                    start_index : start_index + length
                ]
                line += parsed_link
                self.probability_model.feed(parsed_link)
            else:
                frequencies, _, _ = probabilities_to_frequencies(
                    model.get_probabilities(), 32
                )
                line.append(self.arithmetic_decoder.read(frequencies))
                self.probability_model.feed([line[-1]])
            iterations -= 1
        self.storage.store_record(line)
        return line


if __name__ == "__main__":
    window_size = 30
    coder = SmartCoder()
    storage = SlidingWindowRecordStorage(window_size)
    model = LstmLogModel()
    with open("untitled2.txt", mode="r") as f:
        input_text = f.readlines()

    output_streams = [BitOutputStream(open(f"out_{i}", mode="wb")) for i in range(3)]
    encoder = ArithmeticEncoder(32, output_streams[0])
    arithmetic_log_coder = ArithmeticLogEncoder(
        encoder, model, coder, storage, output_streams[1], output_streams[2]
    )
    arithmetic_log_coder.encode_text(input_text)

    for stream in output_streams:
        stream.close()

    model = LstmLogModel()
    storage.drop()

    input_streams = [BitInputStream(open(f"out_{i}", mode="rb")) for i in range(3)]
    decoder = ArithmeticDecoder(32, input_streams[0])
    arithmetic_log_decoder = ArithmeticLogDecoder(
        decoder, model, coder, storage, input_streams[1], input_streams[2]
    )

    with open("decoded.txt", mode="w") as f:
        f.write("\n".join(arithmetic_log_decoder.decode_text()))

    for stream in input_streams:
        stream.close()

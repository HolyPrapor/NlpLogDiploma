# fmt: off
import os
os.chdir("/home/zeliboba/diploma/NlpLogDiploma")

import pyximport; pyximport.install()
import utils.find_subarray as fs
# fmt: on
import numpy as np
from typing import *
from probability_model.model import ModelInterface
from arithmetic_encoder.v2.encoder import ArithmeticEncoder
from arithmetic_encoder.v2.decoder import ArithmeticDecoder
from iostream.output_stream import BitOutputStream
from iostream.input_stream import BitInputStream
from log_model.log_compresser import (
    AbstractRecordStorage,
    AbstractBaseCoder,
    NaiveCoder,
    SlidingWindowRecordStorage,
    SmartCoder,
)
from probability_model.ppm_model.ppm_model import (
    encode as encode_ppm,
    decode as decode_ppm,
)
from tqdm import tqdm


def bitfield(n: int) -> List[int]:
    return [int((n & (1 << (7 - i))) == (1 << (7 - i))) for i in range(8)]


def get_bits(elements: List[int]) -> List[int]:
    cur = []
    for element in elements:
        cur += bitfield(element)
    return cur


class SecondaryEncoder:
    def on_main_encode(self, encoded: List[int]):
        pass

    def on_line_encode_finished(self, line: List[int]):
        pass

    def write(self, token: int):
        raise NotImplementedError()

    def finish(self):
        raise NotImplementedError()


class ArithmeticPPMEncoder(SecondaryEncoder):
    def __init__(
        self,
        integer_stream: BitOutputStream,
        input_file: str,
        output_file: str,
        context_size=3,
        use_bwt=True,
    ) -> None:
        self.integer_stream = integer_stream
        self.input_file = input_file
        self.output_file = output_file
        self.context_size = context_size
        self.use_bwt = use_bwt

    def write(self, token: int):
        for bit in bitfield(token):
            self.integer_stream.write(bit)

    def finish(self):
        self.integer_stream.close()
        encode_ppm(self.input_file, self.output_file, self.context_size, self.use_bwt)


class ArithmeticLSTMEncoder(SecondaryEncoder):
    def __init__(
        self, arithmetic_encoder: ArithmeticEncoder, model: ModelInterface
    ) -> None:
        self.arithmetic_encoder = arithmetic_encoder
        self.model = model

    def on_main_encode(self, encoded: List[int]):
        self.model.feed(encoded)

    def write(self, token: int):
        freqs = self.model.get_frequencies()
        self.arithmetic_encoder.write(freqs, token)
        self.model.feed([token])

    def finish(self):
        self.arithmetic_encoder.finish()


class CombinedLogEncoder:
    def __init__(
        self,
        log_encoder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        lz_output_stream: BitOutputStream,
        auxiliary_output_stream: BitOutputStream,
        secondary_encoder: SecondaryEncoder,
    ) -> None:
        self.log_encoder = log_encoder
        self.storage = storage
        self.lz_output_stream = lz_output_stream
        self.auxiliary_output_stream = auxiliary_output_stream
        self.secondary_encoder = secondary_encoder

    def _write_to_stream(self, elements: List[int], stream: BitOutputStream) -> None:
        for bit in get_bits(elements):
            stream.write(bit)

    def encode_lines(self, lines: List[List[int]]) -> None:
        self._write_to_stream(
            self.log_encoder.encode_int(len(lines), 4), self.auxiliary_output_stream
        )
        for tokens in tqdm(lines, desc="Combined encoding"):
            operations = self.encode(tokens)
            self._write_to_stream(
                self.log_encoder.encode_int(len(operations), 2),
                self.auxiliary_output_stream,
            )
            for bit in operations:
                self.auxiliary_output_stream.write(bit)
        self.secondary_encoder.finish()

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
            if record_length is not None and record_length > 4:
                length = record_length
                operations[iterations] = 0
                encoded = self.log_encoder.encode_link(
                    record_index, record_start_index, length
                )
                self._write_to_stream(encoded, self.lz_output_stream)
                self.secondary_encoder.on_main_encode(line[start : start + length])
            else:
                operations[iterations] = 1
                self.secondary_encoder.write(line[start])
                length = 1
            start += length
            iterations += 1
        self.storage.store_record(line)
        self.secondary_encoder.on_line_encode_finished(line)
        return operations[:iterations]


def create_output_streams(output_prefix):
    return [
        BitOutputStream(open(f"{output_prefix}_{suffix}", mode="wb"))
        for suffix in ["main", "secondary", "auxiliary"]
    ]


def encode(
    input_file: str,
    output_file_prefix: str,
    log_encoder: AbstractBaseCoder = None,
    storage: AbstractRecordStorage = None,
    secondary_encoder: Callable[[BitOutputStream, str, str], SecondaryEncoder] = None,
    use_arithmetic_for_every_file=True,
):
    main, secondary, auxiliary = create_output_streams(output_file_prefix)

    if log_encoder is None:
        log_encoder = NaiveCoder(200)
        # log_encoder = SmartCoder()
    if storage is None:
        storage = SlidingWindowRecordStorage(200)
    if secondary_encoder is None:
        secondary_encoder = ArithmeticPPMEncoder(
            secondary,
            f"{output_file_prefix}_secondary",
            f"{output_file_prefix}_secondary_final",
        )
    else:
        secondary_encoder = secondary_encoder(
            secondary,
            f"{output_file_prefix}_secondary",
            f"{output_file_prefix}_secondary_final",
        )

    with open(input_file, mode="r") as f:
        input_tokens = [
            np.array(
                [ord(ch) for ch in (line[:-1] if line[-1] == "\n" else line)],
                dtype=np.int32,
            )
            for line in f.readlines()
        ]

    combined_log_coder = CombinedLogEncoder(
        log_encoder, storage, main, auxiliary, secondary_encoder
    )

    combined_log_coder.encode_lines(input_tokens)

    main.close()
    secondary.close()
    auxiliary.close()
    if use_arithmetic_for_every_file:
        encode_ppm(f"{output_file_prefix}_main", f"{output_file_prefix}_main_final")
        encode_ppm(
            f"{output_file_prefix}_auxiliary", f"{output_file_prefix}_auxiliary_final"
        )
    else:
        os.rename(f"{output_file_prefix}_main", f"{output_file_prefix}_main_final")
        os.rename(
            f"{output_file_prefix}_auxiliary", f"{output_file_prefix}_auxiliary_final"
        )


class SecondaryDecoder:
    def on_main_decode(self, decoded: List[int]):
        pass

    def on_line_decode_finished(self, line: List[int]):
        pass

    def read(self):
        raise NotImplementedError()

    def finish(self):
        raise NotImplementedError()


class ArithmeticPPMDecoder(SecondaryDecoder):
    def __init__(
        self, encoded: str, decoded: str, context_size=3, use_bwt=True
    ) -> None:
        decode_ppm(encoded, decoded, context_size, use_bwt)
        with open(decoded, mode="rb") as f:
            self.lines = f.read()
        self.pointer = 0

    def read(self):
        self.pointer += 1
        return self.lines[self.pointer - 1]

    def finish(self):
        pass


# TODO: Arithmetic lstm decoder


class CombinedLogDecoder:
    def __init__(
        self,
        log_encoder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        lz_input_stream: BitInputStream,
        auxiliary_input_stream: BitInputStream,
        secondary_decoder: SecondaryDecoder,
    ) -> None:
        self.log_encoder = log_encoder
        self.storage = storage
        self.lz_input_stream = lz_input_stream
        self.auxiliary_input_stream = auxiliary_input_stream
        self.secondary_decoder = secondary_decoder

    def _get_next_int(self, stream: BitInputStream, size=1):
        return self.log_encoder.decode_int_from_stream(stream, size)

    def decode_text(self) -> List[List[int]]:
        res = []
        length = self._get_next_int(self.auxiliary_input_stream, 4)
        for _ in tqdm(range(length), desc="Combined decoding"):
            tokens = self.decode()
            res.append(tokens)
        self.secondary_decoder.finish()
        return res

    # TODO: Remove supersymbol from naive coder?
    def decode(self) -> List[int]:
        iterations = self._get_next_int(self.auxiliary_input_stream, 2)
        line = []
        while iterations > 0:
            mode = self.auxiliary_input_stream.read()
            if mode == 0:
                (
                    record_index,
                    start_index,
                    length,
                ) = self.log_encoder.decode_link_from_stream(self.lz_input_stream)
                parsed_link = self.storage.log_records[record_index][
                    start_index : start_index + length
                ]
                line += parsed_link
                self.secondary_decoder.on_main_decode(parsed_link)
            else:
                line.append(self.secondary_decoder.read())
            iterations -= 1
        self.storage.store_record(line)
        self.secondary_decoder.on_line_decode_finished(line)
        return line


def create_input_streams(encoded_prefix):
    return [
        BitInputStream(open(f"{encoded_prefix}_{suffix}", mode="rb"))
        for suffix in ["main_final", "auxiliary_final"]
    ]


def decode(
    encoded_file_prefix: str,
    output: str,
    log_encoder: AbstractBaseCoder = None,
    storage: AbstractRecordStorage = None,
    secondary_decoder: Callable[[str, str], SecondaryDecoder] = None,
    use_arithmetic_for_every_file=True,
    decoded_prefix=None,
):
    if decoded_prefix is None:
        decoded_prefix = "decoded"
    if use_arithmetic_for_every_file:
        decode_ppm(f"{encoded_file_prefix}_main_final", f"{decoded_prefix}_main_final")
        decode_ppm(
            f"{encoded_file_prefix}_auxiliary_final",
            f"{decoded_prefix}_auxiliary_final",
        )
        main, auxiliary = create_input_streams(decoded_prefix)
    else:
        main, auxiliary = create_input_streams(encoded_file_prefix)

    if log_encoder is None:
        # log_encoder = SmartCoder()
        log_encoder = NaiveCoder(200)
    if storage is None:
        storage = SlidingWindowRecordStorage(200)
    if secondary_decoder is None:
        secondary_decoder = ArithmeticPPMDecoder(
            f"{encoded_file_prefix}_secondary_final", f"{decoded_prefix}_secondary"
        )
    else:
        secondary_decoder = secondary_decoder(
            f"{encoded_file_prefix}_secondary_final", f"{decoded_prefix}_secondary"
        )

    combined_log_decoder = CombinedLogDecoder(
        log_encoder, storage, main, auxiliary, secondary_decoder
    )
    with open(output, mode="w") as f:
        lines = [
            "".join(chr(c) for c in line) for line in combined_log_decoder.decode_text()
        ]
        f.write("\n".join(lines))

    main.close()
    auxiliary.close()


if __name__ == "__main__":
    encode("encode.txt", "out", use_arithmetic_for_every_file=True)
    decode("out", "decoded.txt", use_arithmetic_for_every_file=True)

from time import time
from tempfile import TemporaryDirectory
import filecmp
import os
from typing import *
from arithmetic_log_model.combined_log_model import (
    ArithmeticPPMDecoder,
    ArithmeticPPMEncoder,
    SecondaryDecoder,
    SecondaryEncoder,
    encode as encode_combined_log,
    decode as decode_combined_log,
)
from benchmarks.evaluator import get_stats
from iostream.output_stream import BitOutputStream
from probability_model.ppm_model.ppm_model import (
    encode as encode_ppm,
    decode as decode_ppm,
)
from log_model.log_compresser import (
    AbstractBaseCoder,
    AbstractRecordStorage,
    NaiveCoder,
    SlidingWindowRecordStorage,
    SmartCoder,
    encode as encode_log,
    decode as decode_log,
)


def _check_sanity(i, o, fmt):
    if not filecmp.cmp(i, o):
        print(f"Encode/decode sanity check has failed. {fmt}")


class CombinedLogResult:
    def __init__(
        self,
        coder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        sec_coder: SecondaryEncoder,
        enc_time: float,
        dec_time: float,
        res_size: int,
        entropy: float,
    ) -> None:
        self.coder = coder
        self.storage = storage
        self.sec_coder = sec_coder
        self.enc_time = enc_time
        self.dec_time = dec_time
        self.res_size = res_size
        self.entropy = entropy


class CombinedLogGrid:
    def __init__(
        self,
        base_coder_factories: List[Callable[[int], AbstractBaseCoder]],
        storages: List[AbstractRecordStorage],
        secondary_coder_factories: List[
            Tuple[
                Callable[[BitOutputStream, str, str], SecondaryEncoder],
                Callable[[str, str], SecondaryDecoder],
            ]
        ],
        use_arithmetics: List[bool],
    ) -> None:
        self.base_coder_factories = base_coder_factories
        if self.base_coder_factories is None:
            self.base_coder_factories = [
                lambda _: SmartCoder(),
                lambda x: NaiveCoder(x),
            ]
        self.storages = storages
        if self.storages is None:
            self.storages = [SlidingWindowRecordStorage()]
        self.secondary_coder_factories = secondary_coder_factories
        if self.secondary_coder_factories is None:
            self.secondary_encoder_factories = [
                (
                    lambda os, i, o: ArithmeticPPMEncoder(os, i, o),
                    lambda i, o: ArithmeticPPMDecoder(i, o),
                )
            ]
        self.use_arithmetics = use_arithmetics
        if self.use_arithmetics is None:
            self.use_arithmetics = [False, True]

    def iterate(self, input_file: str) -> Generator[CombinedLogResult, None, None]:
        with TemporaryDirectory() as tmp:
            out = os.path.join(tmp, "out")
            decoded = os.path.join(tmp, "decoded")
            for coder_factory in self.base_coder_factories:
                for storage in self.storages:
                    coder = coder_factory(storage.window_size)
                    for (
                        sec_encoder,
                        sec_decoder,
                    ) in self.secondary_coder_factories:
                        for use_arithmetics in self.use_arithmetics:
                            start = time()
                            encode_combined_log(
                                input_file,
                                out,
                                coder,
                                storage,
                                sec_encoder,
                                use_arithmetics,
                            )
                            encode_time = time() - start
                            storage.drop()
                            entropy, total = get_stats(f"{out}*_final")
                            start = time()
                            decode_combined_log(
                                out,
                                decoded,
                                coder,
                                storage,
                                sec_decoder,
                                use_arithmetics,
                            )
                            decode_time = time() - start
                            _check_sanity(
                                input_file,
                                decoded,
                            )
                            yield CombinedLogResult(
                                coder,
                                storage,
                                sec_encoder,
                                encode_time,
                                decode_time,
                                total,
                                entropy,
                            )

    def format_name(
        coder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        sec_coder: SecondaryEncoder,
        use: bool,
    ):
        return f"Coder: {str(coder)}, Storage: {str(storage)}, SecCoder: {str(sec_coder)}, Use BWT: {str(use)}"


class ArithmeticPPMResult:
    def __init__(
        self,
        context_size: int,
        use_bwt: bool,
        encode_time: float,
        decode_time: float,
        entropy: float,
        size: int,
    ) -> None:
        self.context_size = context_size
        self.use_bwt = use_bwt
        self.encode_time = encode_time
        self.decode_time = decode_time
        self.entropy = entropy
        self.size = size


class ArithmeticPPMGrid:
    def __init__(self, context_sizes: List[int], use_bwts: List[bool]) -> None:
        self.context_sizes = context_sizes
        self.use_bwts = use_bwts

    def iterate(self, input_file: str) -> Generator[ArithmeticPPMResult, None, None]:
        with TemporaryDirectory() as tmp:
            out = os.path.join(tmp, "out")
            decoded = os.path.join(tmp, "decoded")
            for context in self.context_sizes:
                for use_bwt in self.use_bwts:
                    start = time()
                    encode_ppm(input_file, out, context, use_bwt)
                    encode_time = time() - start
                    entropy, total = get_stats(f"{out}*_final")
                    start = time()
                    decode_ppm(out, decoded, context, use_bwt)
                    decode_time = time() - start
                    _check_sanity(
                        input_file, decoded, f"Context: {context}, UseBWT: {use_bwt}"
                    )
                    yield ArithmeticPPMResult(
                        context, use_bwt, encode_time, decode_time, entropy, total
                    )


class LogResult:
    def __init__(
        self,
        coder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        enc_time: float,
        dec_time: float,
        res_size: int,
        entropy: float,
    ) -> None:
        self.coder = coder
        self.storage = storage
        self.enc_time = enc_time
        self.dec_time = dec_time
        self.res_size = res_size
        self.entropy = entropy


class LogGrid:
    def __init__(
        self,
        base_coder_factories: List[Callable[[int], AbstractBaseCoder]],
        storages: List[AbstractRecordStorage],
    ) -> None:
        self.base_coder_factories = base_coder_factories
        if self.base_coder_factories is None:
            self.base_coder_factories = [
                lambda _: SmartCoder(),
                lambda x: NaiveCoder(x),
            ]
        self.storages = storages
        if self.storages is None:
            self.storages = [SlidingWindowRecordStorage()]

    def iterate(self, input_file: str) -> Generator[CombinedLogResult, None, None]:
        with TemporaryDirectory() as tmp:
            out = os.path.join(tmp, "out")
            decoded = os.path.join(tmp, "decoded")
            for coder_factory in self.base_coder_factories:
                for storage in self.storages:
                    coder = coder_factory(storage.window_size)
                    start = time()
                    encode_log(input_file, out, coder, storage)
                    encode_time = time() - start
                    storage.drop()
                    entropy, total = get_stats(f"{out}*_final")
                    start = time()
                    decode_log(out, decoded, coder, storage)
                    decode_time = time() - start
                    _check_sanity(
                        input_file,
                        decoded,
                    )
                    yield LogResult(
                        coder,
                        storage,
                        encode_time,
                        decode_time,
                        total,
                        entropy,
                    )

from time import time
from tempfile import TemporaryDirectory
import filecmp
import os
from typing import *
from arithmetic_log_model.combined_log_model import (
    ArithmeticPPMDecoder,
    ArithmeticPPMEncoder,
    BzipEncoder,
    GzipEncoder,
    RarEncoder,
    SecondaryDecoder,
    SecondaryEncoder,
    encode as encode_combined_log,
    decode as decode_combined_log,
)
from benchmarks.evaluator import get_stats
from iostream.output_stream import BitOutputStream
from v1.probability_model.ppm_model import (
    encode as encode_ppm,
    decode as decode_ppm,
)
from log_model.log_compresser import (
    AbstractBaseCoder,
    AbstractRecordStorage,
    EliasDeltaCoder,
    MoveToFrontStorage,
    MoveToFrontTrieStorage,
    NaiveCoder,
    SlidingWindowRecordStorage,
    SlidingWindowWithDifferentSuffixesRecordStorage,
    SmartCoder,
    TripleCoder,
    UnaryCoder,
    encode as encode_log,
    decode as decode_log,
)


def _check_sanity(i, o, fmt):
    if not filecmp.cmp(i, o, False):
        print(f"Encode/decode sanity check has failed. {fmt}")


class CombinedLogResult:
    def __init__(
        self,
        coder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        sec_coder: SecondaryEncoder,
        bwt_for_all_files: bool,
        enc_time: float,
        dec_time: float,
        res_size: int,
        entropy: float,
    ) -> None:
        self.coder = coder
        self.storage = storage
        self.sec_coder = sec_coder
        self.bwt_for_all_files = bwt_for_all_files
        self.enc_time = enc_time
        self.dec_time = dec_time
        self.res_size = res_size
        self.entropy = entropy

    def __str__(self) -> str:
        # return f"Coder: {self.coder}, Storage: {self.storage}, SecCoder: {self.sec_coder}, BwtForAll: {self.bwt_for_all_files}, ET: {self.enc_time:0.3f}, DT: {self.dec_time:0.3f}, Size: {self.res_size}, Entropy: {self.entropy:0.5f}"
        return f"Coder: {self.coder}, Storage: {self.storage}, SecCoder: {self.sec_coder}, BwtForAll: {self.bwt_for_all_files}, Size: {self.res_size}"


class CombinedLogGrid:
    def __init__(
        self,
        base_coder_factories: List[Callable[[int], AbstractBaseCoder]] = None,
        storages: List[AbstractRecordStorage] = None,
        secondary_coder_factories: List[
            Tuple[
                Callable[[BitOutputStream, str, str], SecondaryEncoder],
                Callable[[str, str], SecondaryDecoder],
            ]
        ] = None,
        use_for_every_file: List[bool] = None,
    ) -> None:
        self.base_coder_factories = base_coder_factories
        if self.base_coder_factories is None:
            self.base_coder_factories = [
                # lambda _: TripleCoder(),
                # lambda _: EliasDeltaCoder(),
                # lambda _: UnaryCoder(),
                # lambda _: SmartCoder(255),
                # lambda _: SmartCoder(),
                lambda x: NaiveCoder(x, False),
                # lambda x: NaiveCoder(x, True),
            ]
        self.storages = storages
        if self.storages is None:
            self.storages = [
                # MoveToFrontStorage(15),
                # MoveToFrontStorage(30),
                MoveToFrontTrieStorage(250)
                # MoveToFrontStorage(50),
                # MoveToFrontStorage(100),
                # MoveToFrontStorage(150),
                # MoveToFrontStorage(200),
                # MoveToFrontStorage(250),
                # SlidingWindowWithDifferentSuffixesRecordStorage(30),
                # SlidingWindowRecordStorage(30),
            ]
        self.secondary_coder_factories = secondary_coder_factories
        if self.secondary_coder_factories is None:
            self.secondary_coder_factories = [
                # (
                #     # lambda os, i, o: BzipEncoder(os, i, o),
                #     # lambda os, i, o: RarEncoder(os, i, o),
                #     lambda os, i, o: ArithmeticPPMEncoder(os, i, o, 0),
                #     lambda i, o: ArithmeticPPMDecoder(i, o),
                # ),
                (
                    # lambda os, i, o: BzipEncoder(os, i, o),
                    # lambda os, i, o: RarEncoder(os, i, o),
                    lambda os, i, o: ArithmeticPPMEncoder(os, i, o, 1),
                    lambda i, o: ArithmeticPPMDecoder(i, o),
                ),
                # (
                #     # lambda os, i, o: BzipEncoder(os, i, o),
                #     # lambda os, i, o: RarEncoder(os, i, o),
                #     lambda os, i, o: ArithmeticPPMEncoder(os, i, o, 3),
                #     lambda i, o: ArithmeticPPMDecoder(i, o),
                # ),
                (
                    lambda os, i, o: BzipEncoder(os, i, o),
                    # lambda os, i, o: RarEncoder(os, i, o),
                    # lambda os, i, o: ArithmeticPPMEncoder(os, i, o),
                    lambda i, o: ArithmeticPPMDecoder(i, o),
                ),
                # (
                #     lambda os, i, o: GzipEncoder(os, i, o),
                #     # lambda os, i, o: RarEncoder(os, i, o),
                #     # lambda os, i, o: ArithmeticPPMEncoder(os, i, o),
                #     lambda i, o: ArithmeticPPMDecoder(i, o),
                # ),
            ]
        self.use_for_every_file = use_for_every_file
        if self.use_for_every_file is None:
            self.use_for_every_file = [True]

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
                        for use in self.use_for_every_file:
                            start = time()
                            storage.drop()
                            encode_combined_log(
                                input_file,
                                out,
                                coder,
                                storage,
                                sec_encoder,
                                use,
                            )
                            encode_time = time() - start
                            entropy, total = get_stats(f"{out}*_final*")
                            start = time()
                            storage.drop()
                            # decode_combined_log(
                            #     out,
                            #     decoded,
                            #     coder,
                            #     storage,
                            #     sec_decoder,
                            #     use_arithmetics,
                            # )
                            decode_time = time() - start
                            # _check_sanity(
                            #     input_file,
                            #     decoded,
                            #     self.format_name(
                            #         coder,
                            #         storage,
                            #         sec_encoder(None, None, None),
                            #         use_arithmetics,
                            #     ),
                            # )
                            yield CombinedLogResult(
                                coder,
                                storage,
                                sec_encoder(None, None, None),
                                use,
                                encode_time,
                                decode_time,
                                total,
                                entropy,
                            )
                            storage.drop()

    def format_name(
        self,
        coder: AbstractBaseCoder,
        storage: AbstractRecordStorage,
        sec_coder: SecondaryEncoder,
        use: bool,
    ):
        return f"Coder: {str(coder)}, Storage: {str(storage)}, SecCoder: {str(sec_coder)}, UseForEveryFile: {str(use)}"


class ArithmeticPPMResult:
    def __init__(
        self,
        context_size: int,
        use_bwt: bool,
        enc_time: float,
        dec_time: float,
        entropy: float,
        res_size: int,
    ) -> None:
        self.context_size = context_size
        self.use_bwt = use_bwt
        self.enc_time = enc_time
        self.dec_time = dec_time
        self.entropy = entropy
        self.res_size = res_size

    def __str__(self) -> str:
        # return f"Context: {self.context_size}, BWT: {self.use_bwt}, ET: {self.enc_time:0.3f}, DT: {self.dec_time:0.3f}, Size: {self.res_size}, Entropy: {self.entropy:0.5f}"
        return (
            f"Context: {self.context_size}, BWT: {self.use_bwt}, Size: {self.res_size}"
        )


class ArithmeticPPMGrid:
    def __init__(
        self, context_sizes: List[int] = None, use_bwts: List[bool] = None
    ) -> None:
        self.context_sizes = context_sizes
        if self.context_sizes is None:
            self.context_sizes = [0, 1, 2, 3, 4, 5]
        self.use_bwts = use_bwts
        if self.use_bwts is None:
            self.use_bwts = [False, True]

    def iterate(self, input_file: str) -> Generator[ArithmeticPPMResult, None, None]:
        with TemporaryDirectory() as tmp:
            out = os.path.join(tmp, "out")
            decoded = os.path.join(tmp, "decoded")
            for context in self.context_sizes:
                for use_bwt in self.use_bwts:
                    start = time()
                    encode_ppm(input_file, out, context, use_bwt)
                    encode_time = time() - start
                    entropy, total = get_stats(out)
                    start = time()
                    # decode_ppm(out, decoded, context, use_bwt)
                    decode_time = time() - start
                    # _check_sanity(
                    #     input_file, decoded, f"Context: {context}, UseBWT: {use_bwt}"
                    # )
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

    def __str__(self) -> str:
        # return f"Coder: {self.coder}, Storage: {self.storage}, ET: {self.enc_time:0.3f}, DT: {self.dec_time:0.3f}, Size: {self.res_size}, Entropy: {self.entropy:0.5f}"
        return f"Coder: {self.coder}, Storage: {self.storage}, Size: {self.res_size}"


class LogGrid:
    def __init__(
        self,
        base_coder_factories: List[Callable[[int], AbstractBaseCoder]] = None,
        storages: List[AbstractRecordStorage] = None,
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
                    storage.drop()
                    encode_log(input_file, out, coder, storage)
                    encode_time = time() - start
                    entropy, total = get_stats(f"{out}*_final")
                    start = time()
                    # storage.drop()
                    # decode_log(out, decoded, coder, storage)
                    decode_time = time() - start
                    # _check_sanity(
                    #     input_file, decoded, f"Coder: {coder}, Storage: {storage}"
                    # )
                    yield LogResult(
                        coder,
                        storage,
                        encode_time,
                        decode_time,
                        total,
                        entropy,
                    )


if __name__ == "__main__":
    import sys

    sys.setrecursionlimit(5000)
    print(sys.getrecursionlimit())
    dir_names = ["medium"]
    for dir_name in dir_names:
        for file_name in ["android", "bgl", "hdfs", "java", "windows"]:
            filepath = "test_files/logs/" + dir_name + "/" + file_name + ".log"
            print(filepath)
            # for result in ArithmeticPPMGrid().iterate(filepath):
            for result in CombinedLogGrid().iterate(filepath):
                # for result in LogGrid().iterate(filepath):
                print(result)

    # for result in CombinedLogGrid().iterate("encode.txt"):
    #     print(result)
    # for result in ArithmeticPPMGrid().iterate("encode.txt"):
    #     print(result)
    # for result in LogGrid().iterate("encode.txt"):
    #     print(result)

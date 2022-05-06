import numpy as np
from probability_model.model import ModelInterface, Token
from iostream.output_stream import BitOutputStream
from iostream.input_stream import BitInputStream
from arithmetic_encoder.v2.encoder import ArithmeticEncoder
from arithmetic_encoder.v2.decoder import ArithmeticDecoder
from utils.bwt_coder import BwtCoder
from typing import *
import os
from tqdm import tqdm

# os.chdir("/home/zeliboba/diploma/NlpLogDiploma")


class Node:
    def __init__(self, length):
        self.length = length
        self.seen = 0
        self.different = 0
        self.children_counts = np.full(length + 1, Trie._minimum_count, dtype=np.int32)
        self.children = [None] * length

    def update(self, buffer, offset, size):
        if size == 0:
            self.seen += 1
            if self.children_counts[buffer[offset]] == Trie._minimum_count:
                self.different += 1
            self.children_counts[buffer[offset]] += 1
        else:
            if self.children[buffer[offset]] is None:
                self.children[buffer[offset]] = Node(self.length)
            self.children[buffer[offset]].update(buffer, offset + 1, size - 1)


# TODO: Implement update exclusions


class Trie:
    _minimum_count = 0

    def __init__(self, length):
        self.length = length
        self.root = Node(length)

    # What to do when we do not have required context? (We can surely go to the lower context level, because it will not cause any ambiguity during decoding)
    # How does this differ with the case when we have the context but do not have a required character in that context? (However, when we have the context, we should explain to the decoder that lower context must be used)
    def try_path(self, buffer, offset, size, encode=True):
        node = self.root
        for i in range(size):
            if node.children[buffer[offset + i]] is None:
                return False, None
            node = node.children[buffer[offset + i]]
        if encode:
            if node.children_counts[buffer[offset + size]] > Trie._minimum_count:
                return True, self._preprocess_counts(node)
            elif node.seen > 0:
                return False, self._preprocess_counts(node)
            return False, None
        if node.seen == 0:
            return False, None
        return True, self._preprocess_counts(node)

    def update(self, buffer, offset, size):
        for context_size in range(size):
            self.root.update(buffer, offset + size - 1 - context_size, context_size)

    def _preprocess_counts(self, parent):
        cum = np.cumsum(parent.children_counts)
        total = cum[-1]
        q = self._get_escape_probability(parent)
        y = np.ceil(q * total / (1 - q)) if q < 1 else 2**32 - total
        cum[-1] = total + y
        return cum

    def _get_escape_probability(self, parent):
        count = parent.seen
        different = parent.different
        return different / (count + different)

    def print(self):
        self.root.print()


class PPMBaseModel(ModelInterface):
    def __init__(self, context_size, alphabet_size=256):
        self.context_size = context_size
        self.alphabet_size = alphabet_size + 1
        self.trie = Trie(self.alphabet_size)
        self.context = np.array([], dtype=np.int32)

    def preprocess(self, text: str) -> List[Token]:
        return [Token(ord(ch)) for ch in text]

    def preprocess_bytes(self, text: bytes) -> List[Token]:
        return [Token(int(b)) for b in text]

    def postprocess(self, tokens: List[Token]) -> str:
        return "".join(chr(token.value) for token in tokens)

    def postprocess_bytes(self, tokens: List[Token]) -> bytes:
        return bytes([t.value for t in tokens])

    def get_eof_token(self) -> Token:
        return Token(self._get_eof_token())

    def _get_esc_token(self) -> Token:
        return self.alphabet_size

    def _get_eof_token(self):
        return self.alphabet_size - 1

    def _get_uniform_frequencies(self):
        return np.cumsum(np.full(self.alphabet_size, 2, dtype=np.int32))


class PPMEncoderModel(PPMBaseModel):
    def __init__(self, coder, context_size, alphabet_size=256):
        super().__init__(context_size, alphabet_size)
        self.coder = coder

    def feed(self, next_tokens: List[Token]):
        if len(next_tokens) != 1:
            raise Exception("Multiple tokens are not supported")
        self.context = (np.concatenate([self.context, next_tokens], dtype=np.int32))[
            -(self.context_size + 1) :
        ]

    def get_frequencies(self) -> List[int]:
        for size in range(min(self.context_size, len(self.context) - 1), -1, -1):
            ok, freqs = self.trie.try_path(
                self.context, len(self.context) - size - 1, size
            )
            if ok:
                self.trie.update(self.context, 0, len(self.context))
                return freqs
            if freqs is not None:
                self.coder.write(freqs, self._get_esc_token())
        self.trie.update(self.context, 0, len(self.context))
        return self._get_uniform_frequencies()


class PPMModelDecoder(PPMBaseModel):
    def __init__(self, decoder, context_size, tokens, alphabet_size=256):
        super().__init__(context_size, alphabet_size)
        self.decoder = decoder
        self.current_context = 0
        self.tokens = tokens

    def feed(self, next_tokens: List[Token]):
        if len(next_tokens) != 1:
            raise Exception("Multiple tokens are not supported")
        token = next_tokens[0]
        while self.current_context >= 0 and token == self._get_esc_token():
            self.current_context -= 1
            freqs = self.get_frequencies()
            token = self.decoder.read(freqs)
        self.tokens.append(Token(token))
        if token == self._get_esc_token():
            raise Exception("How can this be possible? This is surely a bug in code")
        self.context = (np.concatenate([self.context, [token]]))[
            -(self.context_size + 1) :
        ]
        self.trie.update(self.context, 0, len(self.context))
        self.current_context = min(self.context_size, len(self.context))

    def get_frequencies(self) -> List[int]:
        while self.current_context >= 0:
            ok, freqs = self.trie.try_path(
                self.context,
                len(self.context) - self.current_context,
                self.current_context,
                False,
            )
            if ok:
                return freqs
            self.current_context -= 1
        return self._get_uniform_frequencies()


def encode(input_filename, output_filename, context_size=5, use_bwt=True):
    to_encode, bwt_encoded, encoded = fix_file_paths(
        [input_filename, "bwt_encoded", output_filename]
    )
    if use_bwt:
        bwt = BwtCoder()
        bwt.encode(to_encode, bwt_encoded)
    else:
        bwt_encoded = to_encode

    with open(bwt_encoded, mode="rb") as f:
        input_text = f.read()

    output_stream = BitOutputStream(open(encoded, mode="wb"))
    encoder = ArithmeticEncoder(32, output_stream)
    model = PPMEncoderModel(encoder, context_size)
    tokens = model.preprocess_bytes(input_text)
    tokens.append(model.get_eof_token())
    for token in tqdm(tokens, desc="PPM encoding"):
        model.feed([token.value])
        frequencies = model.get_frequencies()
        encoder.write(frequencies, token.value)
    encoder.finish()
    output_stream.close()


def decode(encoded_filename, output_filename, context_size=5, use_bwt=True):
    encoded, bwt_decoded, decoded = fix_file_paths(
        [encoded_filename, "bwt_decoded", output_filename]
    )

    if not use_bwt:
        bwt_decoded = decoded

    input_stream = BitInputStream(open(encoded, mode="rb"))
    decoder = ArithmeticDecoder(32, input_stream)
    tokens = []
    model = PPMModelDecoder(decoder, context_size, tokens)

    while True:
        frequencies = model.get_frequencies()
        current_token = Token(decoder.read(frequencies))
        if current_token == model.get_eof_token():
            break
        model.feed([current_token.value])
        if tokens[-1] == model.get_eof_token():
            tokens.pop()
            break
    input_stream.close()
    text = model.postprocess_bytes(tokens)
    with open(bwt_decoded, mode="wb") as f:
        f.write(text)

    if use_bwt:
        bwt = BwtCoder()
        bwt.decode(bwt_decoded, decoded)


def fix_file_paths(files):
    cwd = os.getcwd()
    return [os.path.join(cwd, file) for file in files]


if __name__ == "__main__":
    encode("out_auxiliary", "out_auxillary_final", 5)
    # decode("out", "decoded.txt", 0)

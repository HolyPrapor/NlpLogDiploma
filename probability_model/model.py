from typing import List, NamedTuple, Tuple
import numpy as np


class Token(NamedTuple):
    value: int


def probabilities_to_frequencies(probabilities: List[float], num_bits: int) -> List[int]:
    return np.cumsum(np.ceil(probabilities * (2 ** num_bits - 3)) + 2).astype(int).tolist()


class ModelInterface:
    def preprocess(self, text: str) -> List[Token]:
        raise NotImplementedError()

    def feed(self, next_tokens: List[Token]):
        raise NotImplementedError()

    # default implementation
    def get_frequencies(self) -> List[List[float]]:
        return probabilities_to_frequencies(self.get_probabilities(), 32)

    def postprocess(self, tokens: List[Token]) -> str:
        raise NotImplementedError()

    def get_eof_token(self) -> Token:
        raise NotImplementedError()

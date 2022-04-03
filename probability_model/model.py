from typing import List, NamedTuple, Tuple


class Token(NamedTuple):
    value: int


class ModelInterface:
    def preprocess(self, text: str) -> List[Token]:
        raise NotImplementedError()

    def get_probabilities(self, tokens: List[Token]) -> List[Tuple[Token, float]]:
        raise NotImplementedError()

    def postprocess(self, tokens: List[Token]) -> str:
        raise NotImplementedError()

    def get_eof_token(self) -> Token:
        raise NotImplementedError()

    def get_left_padding(self, tokens: List[Token], window_size: int) -> List[Token]:
        raise NotImplementedError()

from typing import List, NamedTuple, Tuple


class Token(NamedTuple):
    value: int


class ModelInterface:
    def preprocess(self, text: str) -> List[Token]:
        raise NotImplementedError()

    def feed(self, next_tokens: List[Token]):
        raise NotImplementedError()

    def get_probabilities(self) -> List[List[float]]:
        raise NotImplementedError()

    def postprocess(self, tokens: List[Token]) -> str:
        raise NotImplementedError()

    def get_eof_token(self) -> Token:
        raise NotImplementedError()

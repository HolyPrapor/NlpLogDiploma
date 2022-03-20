from typing import Dict, List, NamedTuple


class Token(NamedTuple):
    value: int


class ModelInterface:
    def preprocess(self, text: str) -> List[Token]:
        raise NotImplementedError()

    def get_frequencies(self, token: Token) -> Dict[Token, int]:
        raise NotImplementedError()

    def postprocess(self, tokens: List[Token]) -> str:
        raise NotImplementedError()

    def get_eof_token(self) -> Token:
        raise NotImplementedError()

    def get_sof_token(self) -> Token:
        raise NotImplementedError()

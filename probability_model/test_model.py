from typing import List, Tuple

from probability_model.model import ModelInterface, Token


class TestModel(ModelInterface):
    def get_left_padding(self, tokens: List[Token], window_size: int) -> List[Token]:
        return tokens

    def __init__(self):
        self.prob = [(Token(1), 0.5), (Token(2), 0.25), (Token(3), 0.125), (Token(4), 0.125)]
        self.token_by_symbol = {'a': Token(1), 'b': Token(2), 'c': Token(3), '': Token(4)}
        self.symbol_by_token = {Token(1): 'a', Token(2): 'b', Token(3): 'c', Token(4): ''}

    # TODO: add generator function
    def preprocess(self, text: str) -> List[Token]:
        tokens = []
        for symbol in text:
            tokens.append(self.token_by_symbol[symbol])
        return tokens

    def get_probabilities(self, token: Token) -> List[Tuple[Token, float]]:
        return self.prob

    def postprocess(self, tokens: List[Token]) -> str:
        text = []
        for token in tokens:
            text.append(self.symbol_by_token[token])
        return ''.join(text)

    def get_eof_token(self) -> Token:
        return Token(4)

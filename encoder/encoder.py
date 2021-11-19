from typing import Tuple

from model import ModelInterface


class ArithmeticEncoder:
    START_SYMBOL = ""

    def __init__(self, model: ModelInterface, text: str):
        # TODO: think about not know all text at start of encoding
        self.model = model
        self.text = text
        self.token_by_symbol = self.model.preprocess(text)

    def encode(self) -> Tuple[float]:
        lower_bound, upper_bound = 0, 1
        prev_symbol = ArithmeticEncoder.START_SYMBOL
        for current_symbol in self.text:
            current_length = upper_bound - lower_bound
            for token, probability in sorted(self.model.get_probabilities(self.token_by_symbol[prev_symbol]).items(),
                                             key=lambda item: -item[1]):
                if token == self.token_by_symbol[current_symbol]:
                    upper_bound = lower_bound + probability * current_length
                    break
                lower_bound += probability * current_length
            prev_symbol = current_symbol
        return lower_bound, upper_bound

    def decode(self, result_lower_bound: float, result_upper_bound: float) -> str:
        # TODO: think about tokens from who i get tokens in decode
        # TODO: think about compare float
        lower_bound = 0
        upper_bound = 1
        tokens = []
        prev_token = self.token_by_symbol[ArithmeticEncoder.START_SYMBOL]
        while lower_bound != result_lower_bound:
            current_length = upper_bound - lower_bound
            for token, probability in sorted(self.model.get_probabilities(prev_token).items(),
                                             key=lambda item: -item[1]):
                if (lower_bound <= result_lower_bound and
                        lower_bound + probability * current_length >= result_upper_bound):
                    prev_token = token
                    tokens.append(token)
                    upper_bound = lower_bound + probability * current_length
                    break
                lower_bound += probability * current_length
        return self.model.postprocess(tokens)


class TestModel(ModelInterface):
    def __init__(self):
        pass

    def preprocess(self, text):
        return {'a': 1, 'b': 2, 'c': 3, '': 4}

    def get_probabilities(self, token):
        return {1: 0.6, 2: 0.2, 3: 0.15, 4: 0.05}

    def postprocess(self, tokens):
        symbols_by_token = {1: 'a', 2: 'b', 3: 'c', 4: ''}
        symbols = []
        for token in tokens:
            symbols.append(symbols_by_token[token])
        return ''.join(symbols)


s = "ccbaccbaccaccb"
encoder = ArithmeticEncoder(TestModel(), s)
res = encoder.encode()
print(res)
print(encoder.decode(*res))
print(s == encoder.decode(*res))

from collections import Counter
from typing import Tuple
# from bigfloat import *
from decimal import *
from model import ModelInterface
import random
import pickle


class ArithmeticEncoder:
    START_SYMBOL = ""

    def __init__(self, model: ModelInterface, text: str):
        # TODO: think about not know all text at start of encoding
        self.model = model
        self.text = text
        self.token_by_symbol = self.model.preprocess(text)

    def encode(self) -> Tuple[float]:
        # if model.get_probabilities(a) == model.get_probabilities(a)
        # if index of required token = 3
        # we need only prefix sum 0 2
        lower_bound, upper_bound = Decimal(0), Decimal(1)
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
        lower_bound = Decimal(0)
        upper_bound = Decimal(1)
        tokens = []
        prev_token = self.token_by_symbol[ArithmeticEncoder.START_SYMBOL]
        while lower_bound.compare(result_lower_bound) != 0 or upper_bound.compare(result_upper_bound) != 0:
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
        return {1: Decimal(0.6), 2: Decimal(0.2), 3: Decimal(0.15), 4: Decimal(0.13)}

    def postprocess(self, tokens):
        symbols_by_token = {1: 'a', 2: 'b', 3: 'c', 4: ''}
        symbols = []
        for token in tokens:
            symbols.append(symbols_by_token[token])
        return ''.join(symbols)


def get_symbol(rand):
    if rand < 0.3:
        return 'a'
    if rand < 0.6:
        return 'b'
    return 'c'


def benchmark(n):
    for _ in range(n):
        amount = random.randint(10000, 50000)
        s = ''.join([get_symbol(random.random()) for i in range(amount)])
        getcontext().prec = amount

        print(amount, Counter(s))

        # print(s)
        encoder = ArithmeticEncoder(TestModel(), s)
        res = encoder.encode()
        # print(res)
        # print(encoder.decode(*res))
        print(s == encoder.decode(*res))
        with open('test.bin', 'wb') as f:
            pickle.dump(res, f)
        with open('test.ex', 'wb') as f:
            pickle.dump(s, f)


benchmark(1)


# 11100101001
# 11100100000

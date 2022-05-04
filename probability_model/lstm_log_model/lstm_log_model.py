from typing import List, Dict, Tuple
from os import path

import numpy as np
from probability_model.model import ModelInterface, Token
from utils.rolling_hash import RollingHash
import youtokentome as yttm
import tensorflow as tf
import logging
tf.get_logger().setLevel(logging.ERROR)

# Do we really have to store tokens? Maybe just ints?


class LstmLogModel(ModelInterface):
    def __init__(self, yttm_model_path=None, lstm_model_path=None):
        cur_dir = path.dirname(__file__)
        yttm_model_path = path.join(
            cur_dir, "yttm.model") if not yttm_model_path else yttm_model_path
        lstm_model_path = path.join(
            cur_dir, "lstm") if not lstm_model_path else lstm_model_path

        self.bpe = yttm.BPE(model=yttm_model_path)
        self.model = tf.keras.models.load_model(lstm_model_path)
        self.context = np.array([0 for _ in range(19)])
#         self.hash = RollingHash(2500, 19)

    def feed(self, next_tokens: List[Token]):
        self.context = np.roll(self.context, -len(next_tokens))
        self.context[-len(next_tokens):] = next_tokens[-len(self.context):]
#         for next_token in next_tokens[-19:]:
#             self.hash.roll(next_token, self.context[0])
#             self.context = np.roll(self.context, -1)
#             self.context[-1] = next_token

    def preprocess(self, text: str) -> List[Token]:
        tokens = self.bpe.encode(text, output_type=yttm.OutputType.ID)
        return self.__encode_tokens(tokens)

    def get_probabilities(self) -> List[Tuple[Token, float]]:
        return self.model.predict(np.expand_dims(self.context, axis=0))[0]

    def postprocess(self, tokens: List[Token]) -> str:
        return ''.join(self.bpe.decode(self.__decode_tokens(tokens)))

    def get_eof_token(self) -> Token:
        return Token(3)

    def __encode_tokens(self, tokens: List[int]) -> List[Token]:
        return [Token(x) for x in tokens]

    def __decode_tokens(self, tokens: List[Token]) -> List[int]:
        return list(x.value for x in tokens)

#     def __hash__(self):
#         return hash(self.hash)

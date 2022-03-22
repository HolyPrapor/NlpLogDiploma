from typing import List, Dict

from model.model import ModelInterface, Token
import youtokentome as yttm
import tensorflow as tf

# Do we really have to store tokens? Maybe just ints?


class LstmLogModel(ModelInterface):
    def get_left_padding(self, tokens: List[Token], window_size: int) -> List[Token]:
        return [0 for _ in range(len(tokens) - window_size)] + tokens

    def __init__(self, yttm_model_path=None, lstm_model_path=None):
        yttm_model_path = "yttm.model" if not yttm_model_path else yttm_model_path
        lstm_model_path = "lstm" if not lstm_model_path else lstm_model_path
        self.bpe = yttm.BPE(model=yttm_model_path)
        self.model = tf.keras.models.load_model(lstm_model_path)

    # TODO: add generator function
    def preprocess(self, text: str) -> List[Token]:
        return self.bpe.encode(text, output_type=yttm.OutputType.ID, eos=True)

    def get_probabilities(self, tokens: List[Token]) -> Dict[Token, float]:
        result = self.model.predict(self.__decode_tokens(tokens))[0]
        return {Token(k): v for k, v in enumerate(result)}

    def postprocess(self, tokens: List[Token]) -> str:
        return self.bpe.decode(self.__decode_tokens(tokens))

    def get_eof_token(self) -> Token:
        return Token(3)

    def __decode_tokens(self, tokens: List[Token]) -> List[int]:
        return [x.value for x in tokens]

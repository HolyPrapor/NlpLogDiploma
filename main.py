from encoder.v2.encoder import ArithmeticEncoder
from encoder.v2.decoder import ArithmeticDecoder
from model.lstm_log_model.lstm_log_model import LstmLogModel
from iostream.input_stream import BitInputStream
from iostream.output_stream import BitOutputStream
from argparse import ArgumentParser, Namespace
from model.model import ModelInterface, Token
from typing import Dict
from math import ceil

ENCODE = 'encode'
DECODE = 'decode'


def parse_args():
    parser = ArgumentParser(description='Process arguments for program, which encode and decode your files')
    parser.add_argument('-i', '--input', type=str, required=True, help='Input file, which we want to encode or decode')
    parser.add_argument('-o', '--output', type=str, default='result',
                        help='Output file, in which we want to store results of encoding or decoding')
    parser.add_argument('-n', '--num-bits', type=int, default=32,
                        help='Number of bits for representation range in arithmetic coding')
    parser.add_argument('-m', '--mode', type=str, choices=[ENCODE, DECODE], required=True,
                        help='Mode, in which we want to process input file')
    parser.add_argument('-e', '--encoding', default='utf-8', help='Encoding, in which stored input file')
    parser.add_argument('-w', '--window-size', type=int, default=1, help='Size, what use model for predict frequencies')
    return parser.parse_args()


def main(args: Namespace) -> None:
    model = LstmLogModel()
    if args.mode == ENCODE:
        encode(model, args.input, args.output, args.num_bits, args.encoding, args.window_size)
    if args.mode == DECODE:
        decode(model, args.input, args.output, args.num_bits, args.encoding, args.window_size)


def probabilities_to_frequencies(probabilities: Dict[Token, float], num_bits: int) -> Dict[Token, int]:
    frequencies = {}
    for token in probabilities:
        left = frequencies[Token(token.value - 1)] if token.value > 1 else 0
        frequencies[token] = left + ceil((2 ** num_bits - 1) * probabilities[token])
    return frequencies


def encode(model: ModelInterface, input_file: str, output_file: str, num_bits: int, encoding: str,
           window_size: int) -> None:
    with open(input_file, mode='r', encoding=encoding) as f:
        input_text = '\n'.join(f.readlines())
    output_stream = BitOutputStream(open(output_file, mode='wb'))
    encoder = ArithmeticEncoder(num_bits, output_stream)
    tokens = model.preprocess(input_text)
    tokens.append(model.get_eof_token())
    for i in range(len(tokens)):
        encoder.write(probabilities_to_frequencies(model.get_probabilities(model.get_left_padding(
            tokens[i-window_size+1:i+1], window_size)), num_bits), tokens[i])
    encoder.finish()
    output_stream.close()


def decode(model: ModelInterface, input_file: str, output_file: str, num_bits: int, encoding: str,
           window_size: int) -> None:
    input_stream = BitInputStream(open(input_file, mode='rb'))
    decoder = ArithmeticDecoder(num_bits, input_stream)
    tokens = []
    while True:
        current_token = decoder.read(probabilities_to_frequencies(model.get_probabilities(model.get_left_padding(
            tokens[-window_size:], window_size)), num_bits))
        if current_token == model.get_eof_token():
            break
        tokens.append(current_token)
    input_stream.close()
    text = model.postprocess(tokens)
    with open(output_file, mode='w', encoding=encoding) as f:
        f.write(text)


if __name__ == '__main__':
    main(parse_args())

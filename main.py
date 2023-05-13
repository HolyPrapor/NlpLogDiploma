from arithmetic_encoder.v2.encoder import ArithmeticEncoder
from arithmetic_encoder.v2.decoder import ArithmeticDecoder
from probability_model.lstm_log_model.lstm_log_model import LstmLogModel
from iostream.input_stream import BitInputStream
from iostream.output_stream import BitOutputStream
from argparse import ArgumentParser, Namespace
from probability_model.model import ModelInterface, Token
from typing import List, Tuple, Dict
from math import ceil
from time import time
import numpy as np

ENCODE = 'encode'
DECODE = 'decode'


def parse_args():
    parser = ArgumentParser(
        description='Process arguments for program, which encode and decode your files')
    parser.add_argument('-i', '--input', type=str, required=True,
                        help='Input file, which we want to encode or decode')
    parser.add_argument('-o', '--output', type=str, default='result',
                        help='Output file, in which we want to store results of encoding or decoding')
    parser.add_argument('-n', '--num-bits', type=int, default=32,
                        help='Number of bits for representation range in arithmetic coding')
    parser.add_argument('-m', '--mode', type=str, choices=[ENCODE, DECODE], required=True,
                        help='Mode, in which we want to process input file')
    parser.add_argument('-e', '--encoding', default='utf-8',
                        help='Encoding, in which stored input file')
    parser.add_argument('-tf', '--tensorflow', type=str,
                        default=None, help='Path to tensorflow model to use')
    parser.add_argument('-yttm', '--youtokentome', type=str,
                        default=None, help='Path to youtokentome model to use')
    return parser.parse_args()


def main(args: Namespace) -> None:
    model = LstmLogModel(args.youtokentome, args.tensorflow)
    if args.mode == ENCODE:
        encode(model, args.input, args.output, args.num_bits, args.encoding)
    if args.mode == DECODE:
        decode(model, args.input, args.output, args.num_bits, args.encoding)


def encode(model: ModelInterface, input_file: str, output_file: str, num_bits: int, encoding: str) -> None:
    with open(input_file, mode='r', encoding=encoding) as f:
        input_text = '\n'.join(f.readlines())
    output_stream = BitOutputStream(open(output_file, mode='wb'))
    encoder = ArithmeticEncoder(num_bits, output_stream)
    tokens = model.preprocess(input_text)
    tokens.append(model.get_eof_token())
    for i in range(len(tokens)):
        frequencies = model.get_frequencies()
        encoder.write(frequencies, tokens[i].value)
        model.feed([tokens[i].value])
    encoder.finish()
    output_stream.close()


def decode(model: ModelInterface, input_file: str, output_file: str, num_bits: int, encoding: str) -> None:
    input_stream = BitInputStream(open(input_file, mode='rb'))
    decoder = ArithmeticDecoder(num_bits, input_stream)
    tokens = []
    while True:
        frequencies = model.get_frequencies()
        current_token = Token(decoder.read(frequencies))
        if current_token == model.get_eof_token():
            break
        tokens.append(current_token)
        model.feed([current_token.value])
    input_stream.close()
    text = model.postprocess(tokens)
    with open(output_file, mode='w', encoding=encoding) as f:
        f.write(text)


if __name__ == '__main__':
    start = time()
    main(parse_args())
    end = time()
    print(f"Execution time: {end - start}")

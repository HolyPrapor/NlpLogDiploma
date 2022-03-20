from encoder.v2.encoder import ArithmeticEncoder
from encoder.v2.decoder import ArithmeticDecoder
from model.test_model import TestModel
from iostream.input_stream import BitInputStream
from iostream.output_stream import BitOutputStream
from argparse import ArgumentParser, Namespace
from model.model import ModelInterface

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
    return parser.parse_args()


def main(args: Namespace) -> None:
    model = TestModel()
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
    for token in tokens:
        encoder.write(model.get_frequencies(token), token)
    encoder.finish()
    output_stream.close()


def decode(model: ModelInterface, input_file: str, output_file: str, num_bits: int, encoding: str) -> None:
    input_stream = BitInputStream(open(input_file, mode='rb'))
    decoder = ArithmeticDecoder(num_bits, input_stream)
    tokens = [model.get_sof_token()]
    while True:
        current_token = decoder.read(model.get_frequencies(tokens[-1]))
        if current_token == model.get_eof_token():
            break
        tokens.append(current_token)
    input_stream.close()
    text = model.postprocess(tokens)
    with open(output_file, mode='w', encoding=encoding) as f:
        f.write(text)


if __name__ == '__main__':
    main(parse_args())

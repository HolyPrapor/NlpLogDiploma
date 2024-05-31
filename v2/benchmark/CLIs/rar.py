import os

from v2.benchmark.common import CompressionCLI, calculate_entropy


class Rar(CompressionCLI):
    def __init__(self, **kwargs):
        self.params = kwargs

    def compress_command(self, input_file, output_dir):
        return f"rar a {output_dir}/{os.path.basename(input_file)}.rar {input_file}"

    def decompress_command(self, input_file, output_dir):
        return f"rar x {output_dir}/{os.path.basename(input_file)}.rar {output_dir}"

    def compressed_size(self, input_file, output_dir):
        return os.path.getsize(f"{output_dir}/{os.path.basename(input_file)}.rar")

    def entropy(self, input_file, output_dir):
        return calculate_entropy([f"{output_dir}/{os.path.basename(input_file)}.rar"])

    def __str__(self):
        return "rar"
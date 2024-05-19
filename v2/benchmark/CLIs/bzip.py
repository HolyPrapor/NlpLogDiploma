import os

from v2.benchmark.common import CompressionCLI, calculate_entropy


class Bzip2(CompressionCLI):
    def __init__(self, **kwargs):
        self.params = kwargs

    def compress_command(self, input_file, output_dir):
        return f"bzip2 -9c {input_file} > {output_dir}/{os.path.basename(input_file)}.bz2"

    def decompress_command(self, input_file, output_dir):
        return f"bzip2 -dc {output_dir}/{os.path.basename(input_file)}.bz2 > {output_dir}/{os.path.basename(input_file)}"

    def compressed_size(self, input_file, output_dir):
        return os.path.getsize(f"{output_dir}/{os.path.basename(input_file)}.bz2")

    def entropy(self, input_file, output_dir):
        return calculate_entropy([f"{output_dir}/{os.path.basename(input_file)}.bz2"])

    def __str__(self):
        return "bzip2"

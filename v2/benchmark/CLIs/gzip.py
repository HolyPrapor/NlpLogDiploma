import os

from v2.benchmark.common import CompressionCLI, calculate_entropy


class Gzip(CompressionCLI):
    def __init__(self, **kwargs):
        self.params = kwargs

    def compress_command(self, input_file, output_dir):
        compression_level = self.params.get('compression_level', 9)
        return f"gzip -{compression_level} {input_file} -c > {output_dir}/{os.path.basename(input_file)}.gz"

    def decompress_command(self, input_file, output_dir):
        return f"gzip -d {output_dir}/{os.path.basename(input_file)}.gz"

    def compressed_size(self, input_file, output_dir):
        return os.path.getsize(f"{output_dir}/{os.path.basename(input_file)}.gz")

    def entropy(self, input_file, output_dir):
        return calculate_entropy([f"{output_dir}/{os.path.basename(input_file)}.gz"])

    def __str__(self):
        return "gzip"
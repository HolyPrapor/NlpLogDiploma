import os

from v2.benchmark.common import CompressionCLI, calculate_entropy


class Zstd(CompressionCLI):
    def __init__(self, **kwargs):
        self.params = kwargs

    def compress_command(self, input_file, output_dir):
        compression_level = self.params.get('compression_level', 3)
        return f"zstd -{compression_level} -c {input_file} > {output_dir}/{os.path.basename(input_file)}.zst"

    def decompress_command(self, input_file, output_dir):
        return f"zstd -dc {output_dir}/{os.path.basename(input_file)}.zst > {output_dir}/{os.path.basename(input_file)}"

    def compressed_size(self, input_file, output_dir):
        return os.path.getsize(f"{output_dir}/{os.path.basename(input_file)}.zst")

    def entropy(self, input_file, output_dir):
        return calculate_entropy([f"{output_dir}/{os.path.basename(input_file)}.zst"])

    def __str__(self):
        return "zstd"
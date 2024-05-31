import os

from v2.benchmark.common import CompressionCLI, calculate_entropy


class Sevenzip(CompressionCLI):
    def __init__(self, **kwargs):
        self.params = kwargs

    def compress_command(self, input_file, output_dir):
        return f"7z a {output_dir}/{os.path.basename(input_file)}.7z {input_file}"

    def decompress_command(self, input_file, output_dir):
        return f"7z x {output_dir}/{os.path.basename(input_file)}.7z -o{output_dir}"

    def compressed_size(self, input_file, output_dir):
        return os.path.getsize(f"{output_dir}/{os.path.basename(input_file)}.7z")

    def entropy(self, input_file, output_dir):
        return calculate_entropy([f"{output_dir}/{os.path.basename(input_file)}.7z"])

    def __str__(self):
        return "7zip"


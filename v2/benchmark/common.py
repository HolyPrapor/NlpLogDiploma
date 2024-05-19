import math
import subprocess
import os
import tempfile
import time
from collections import Counter
from concurrent.futures import ThreadPoolExecutor, as_completed
from abc import ABC, abstractmethod


class CompressionResult:
    def __init__(self, algo, filename, compress_ms, decompress_ms, compression_ratio, entropy):
        self.compressMs = compress_ms
        self.decompressMs = decompress_ms
        self.compressionRatio = compression_ratio
        self.entropy = entropy
        self.algo = algo
        self.filename = filename

    def __str__(self):
        return f"{self.algo} - {self.filename}: Compress: {self.compressMs:.0f}ms, Decompress: {self.decompressMs:.0f}ms, Compression Ratio: {self.compressionRatio:.3f}, Entropy: {self.entropy:.3f}"


class CompressionCLI(ABC):
    @abstractmethod
    def compress_command(self, input_file, output_dir):
        pass

    @abstractmethod
    def decompress_command(self, input_file, output_dir):
        pass

    @abstractmethod
    def compressed_size(self, input_file, output_dir):
        pass

    @abstractmethod
    def entropy(self, input_file, output_dir):
        pass

    @abstractmethod
    def __str__(self):
        pass


def load_dataset(dataset_path):
    """Loads dataset files from the given directory."""
    dataset = {}
    for d in os.listdir(dataset_path):
        dataset[d] = [os.path.join(dataset_path, d, f) for f in os.listdir(os.path.join(dataset_path, d)) if os.path.isfile(os.path.join(dataset_path, d, f))]
    return dataset


def calculate_entropy(filenames):
    counts = Counter()
    total = 0
    for filename in filenames:
        with open(filename, mode="rb") as f:
            data = f.read()
            counts.update(data)
            total += len(data)
    entropy = 0
    for count in counts.values():
        p = count / total
        entropy -= p * math.log(p, 256)
    return entropy


def save_results(results, path):
    """Saves results as csv to the specified path."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w') as f:
        f.write("Algorithm,Filename,CompressMs,DecompressMs,CompressionRatio,Entropy\n")
        for result in results:
            f.write(f"{result.algo},{result.filename},{result.compressMs},{result.decompressMs},{result.compressionRatio},{result.entropy}\n")


def run_command_with_temp_dir(cli, test_type, input_file):
    """Runs the CLI command with a temporary directory and cleans up after execution."""
    with tempfile.TemporaryDirectory() as temp_dir:
        compress_cmd = cli.compress_command(input_file, temp_dir)
        start = time.time()
        subprocess.run(compress_cmd, shell=True, check=True)
        compress_ms = (time.time() - start) * 1000

        decompress_cmd = cli.decompress_command(input_file, temp_dir)
        start = time.time()
        subprocess.run(decompress_cmd, shell=True, check=True)
        decompress_ms = (time.time() - start) * 1000

        compressed_size = cli.compressed_size(input_file, temp_dir)
        original_size = os.path.getsize(input_file)
        compression_ratio = original_size / compressed_size if compressed_size != 0 else 0
        entropy = cli.entropy(input_file, temp_dir)

        formatted_filename = os.path.basename(input_file)
        return CompressionResult(str(cli), f"{test_type}-{formatted_filename}", compress_ms, decompress_ms, compression_ratio, entropy)


def run_commands_in_parallel(clis, dataset_path, max_workers=6):
    """Runs a list of commands in parallel and returns the results."""
    results = []

    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        dataset = load_dataset(dataset_path)
        futures = {executor.submit(run_command_with_temp_dir, cli, test_type, input_file): (cli, test_type, input_file) for cli in clis for test_type, input_files in dataset.items() for input_file in input_files}

        for future in as_completed(futures):
            command = futures[future]
            try:
                output = future.result()
                results.append(output)
                print(output)
            except Exception as e:
                print(f'Error with command {command}: {e}')

    return results

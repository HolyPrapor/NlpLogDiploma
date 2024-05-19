import os
from v2.benchmark.common import CompressionCLI, calculate_entropy


class SubPrePCS(CompressionCLI):
    def __init__(self, params=None):
        self.executable = "../../build/cli"
        self.params = params or {}

    def compress_command(self, input_file, output_dir):
        config_file = self.create_config_file(output_dir)
        filename = os.path.splitext(os.path.basename(input_file))[0]
        output_prefix = os.path.join(output_dir, filename)
        return f"cat {config_file} | {self.executable} compress {input_file} {output_prefix}"

    def decompress_command(self, input_file, output_dir):
        config_file = self.create_config_file(output_dir)
        filename = os.path.splitext(os.path.basename(input_file))[0]
        output_prefix = os.path.join(output_dir, filename)
        return f"cat {config_file} | {self.executable} decompress {output_prefix} {input_file}"

    def compressed_size(self, input_file, output_dir):
        filename = os.path.splitext(os.path.basename(input_file))[0]
        primary = os.path.join(output_dir, f'{filename}_primary')
        secondary = os.path.join(output_dir, f'{filename}_secondary')
        markup = os.path.join(output_dir, f'{filename}_markup')
        return os.path.getsize(primary) + os.path.getsize(secondary) + os.path.getsize(markup)

    def entropy(self, input_file, output_dir):
        filename = os.path.splitext(os.path.basename(input_file))[0]
        primary = os.path.join(output_dir, f'{filename}_primary')
        secondary = os.path.join(output_dir, f'{filename}_secondary')
        markup = os.path.join(output_dir, f'{filename}_markup')
        return calculate_entropy([primary, secondary, markup])

    def create_config_file(self, output_dir):
        config_file_path = os.path.join(output_dir, 'compression_config.textproto')
        save_textproto(self.params, config_file_path)
        return config_file_path

    def __str__(self):
        # remove all commas and newlines
        return f"SubPrePCS({self.params})".replace(',', '').replace('\n', '')


def dict_to_textproto(d, indent=0):
    """Recursively formats a dictionary into textproto format."""
    lines = []
    for key, value in d.items():
        if isinstance(value, dict):
            lines.append(f"{' ' * indent}{key} {{")
            lines.append(dict_to_textproto(value, indent + 2))
            lines.append(f"{' ' * indent}}}")
        elif isinstance(value, list):
            for item in value:
                if isinstance(item, dict):
                    lines.append(f"{' ' * indent}{key} {{")
                    lines.append(dict_to_textproto(item, indent + 2))
                    lines.append(f"{' ' * indent}}}")
                else:
                    lines.append(f"{' ' * indent}{key}: {item}")
        else:
            lines.append(f"{' ' * indent}{key}: {value}")
    return "\n".join(lines)


def save_textproto(config, filepath):
    """Saves a dictionary as a textproto file."""
    textproto_str = dict_to_textproto(config)
    with open(filepath, 'w') as f:
        f.writelines(["# proto-file: compression_config.proto\n",
                      "# proto-message: CompressionConfig\n",
                      "\n"])
        f.write(textproto_str)

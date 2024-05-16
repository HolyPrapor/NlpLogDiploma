# Usage

`./cli compress <input_file> <output_prefix>` to compress
`./cli decompress <input_prefix> <output_file>` to decompress

It's possible to specify algorithm parameters through a `.textproto` file.
Check out `compression_config.proto` for the available options.
Run with `cat my_config.textproto | ./cli compress <input_file> <output_prefix>` to use a custom configuration.

# Required steps to work with the project

1. Install cmake (sudo apt-get install cmake)
2. Install Protobuf (sudo apt-get install libprotobuf-dev protobuf-compiler)
3. Install Catch2 (https://github.com/etaler/Etaler/issues/33#issuecomment-508972564)
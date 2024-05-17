import common

# List of CLIs to benchmark with parameters
cli_commands = [
    {'executable': './my_compression_cli', 'params': {'param1': 1, 'param2': 0.1, 'param3': 'option1'}},
    {'executable': './another_compression_cli', 'params': {'paramA': 1, 'paramB': 0.1, 'paramC': 'option1'}},
    {'executable': './yet_another_compression_cli', 'params': {'opt1': 1, 'opt2': 0.1, 'opt3': 'option1'}}
]

def benchmark_clis(input_dir):
    cli_instances = create_cli_instances(cli_commands)
    results = common.run_commands_in_parallel(cli_instances, input_dir)
    common.save_results(results, 'benchmark_results.csv')


if __name__ == '__main__':
    input_directory = '../../test_files'
    benchmark_clis(input_directory)

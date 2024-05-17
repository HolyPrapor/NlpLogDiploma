import itertools
import common

# Define your hyperparameters grid
param_grid = {
    'param1': [1, 2, 3],
    'param2': [0.1, 0.2],
    'param3': ['option1', 'option2']
}

# Generate all combinations of parameters
param_combinations = list(itertools.product(*param_grid.values()))


def create_cli_instances(param_combinations):
    """Creates CLI instances for all parameter combinations."""
    cli_instances = []
    for params in param_combinations:
        params_dict = {key: value for key, value in zip(param_grid.keys(), params)}
        cli_instances.append(MyCompressionCLI('./my_compression_cli', params_dict))
    return cli_instances


def hyperparameter_search(input_dir):
    cli_instances = create_cli_instances(param_combinations)
    results = common.run_commands_in_parallel(cli_instances, input_dir)
    common.save_results(results, 'hyperparameter_search_results.csv')


if __name__ == '__main__':
    input_directory = '../../test_files'
    hyperparameter_search(input_directory)

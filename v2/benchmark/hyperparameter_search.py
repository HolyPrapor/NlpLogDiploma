import itertools
import os
from datetime import datetime

import common
from v2.benchmark.CLIs.subprepcs import SubPrePCS


def create_param_grid():
    return {
        "primary_log_coder": {
            "storage_size": [2, 5],
            "storage": [
                {"greedy_sliding_window_storage": {}},
                {"greedy_move_to_front_storage": {}}
            ]
        },
        'generic_primary_coder': {
            'coder': [{
                "modelling_coder": {
                    "context_size": [2, 3]
                },
            },
                {"identity_coder": {}}
            ],
        },
    }


def expand_nested_combinations(nested_param_grid):
    """Generates all combinations for a nested parameter grid."""
    expanded_combinations = [{}]

    for key, sub_grid in nested_param_grid.items():
        if isinstance(sub_grid, dict):
            sub_combinations = expand_nested_combinations(sub_grid)
            expanded_combinations = [dict(**base, **{key: sub}) for base in expanded_combinations for sub in sub_combinations]
        else:
            expanded_combinations = [dict(**base, **{key: value}) for base in expanded_combinations for value in sub_grid]

    return expanded_combinations


def create_cli_instances():
    """Creates CLI instances for all parameter combinations."""
    nested_combinations = expand_nested_combinations(create_param_grid())

    cli_instances = []
    for params in nested_combinations:
        cli_instances.append(SubPrePCS(params))

    return cli_instances


def hyperparameter_search(input_dir, output_dir):
    formatted_now = datetime.now().strftime("%Y%m%d_%H%M%S")
    cli_instances = create_cli_instances()
    results = common.run_commands_in_parallel(cli_instances, input_dir)
    output = os.path.join(output_dir, f'hyperparameter_search_results-{formatted_now}.csv')
    common.save_results(results, output)


if __name__ == '__main__':
    input_directory = '../../test_files/logs'
    output_directory = '/tmp/subprepcs'
    hyperparameter_search(input_directory, output_directory)



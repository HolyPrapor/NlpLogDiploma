import os
from datetime import datetime

import common
from CLIs.subprepcs import SubPrePCS
from CLIs.bzip import Bzip2


def create_cli_instances():
    test_instance_params = {
        "primary_log_coder": {
            "storage_size": 255,
            "min_link_length": 10,
            "greedy_move_to_front_storage": {
            }
        },
        "secondary_log_coder": {
            "ppm_secondary_log_coder": {
                "window_size": 5
            }
        },
        "generic_primary_coder": {
            "bwt_modelling_coder": {
                "context_size": 3,
                "chunk_size": 1000000
            }
        },
        "generic_secondary_coder": {
            "identity_coder": {}
        },
        "generic_markup_coder": {
            "bwt_modelling_coder": {
                "context_size": 2,
                "chunk_size": 1000000
            }
        }
    }
    return [Bzip2(), SubPrePCS(), SubPrePCS(test_instance_params)]


def benchmark_clis(input_dir, output_dir):
    formatted_now = datetime.now().strftime("%Y%m%d_%H%M%S")
    cli_instances = create_cli_instances()
    results = common.run_commands_in_parallel(cli_instances, input_dir, 2)
    output = os.path.join(output_dir, f'benchmark_results-{formatted_now}.csv')
    common.save_results(results, output)


if __name__ == '__main__':
    input_directory = '../../test_files/logs'
    output_directory = "results"
    benchmark_clis(input_directory, output_directory)

import os
from datetime import datetime

import common
from CLIs.subprepcs import SubPrePCS
from CLIs.bzip import Bzip2
from CLIs.zstd import Zstd
from CLIs.zpaq import Zpaq
from CLIs.sevenzip import Sevenzip
from CLIs.rar import Rar
from CLIs.gzip import Gzip


def create_cli_instances():
    test_instance_params = {
        'primary_log_coder': {
            'residue_log_link_coder': {},
            'greedy_move_to_front_2_storage': {
                'static_movement_degree': 1,
                "stable": False
            },
            'storage_size': 1020,
            'min_link_length': 3,
            'ngram_log_filter': {
                'n': 1,
                'acceptance_threshold': 0.55,
            },
            "use_optimal_partitioning": False
        },
        'secondary_log_coder': {
            'residue_secondary_log_coder': {
            }
        },
        'generic_primary_coder': {
            'zstd_coder': {
                'compression_level': 22
            }
        },
        'generic_secondary_coder': {
            'zstd_coder': {
                'compression_level': 22
            }
        },
        'generic_markup_coder': {
            'zstd_coder': {
                'compression_level': 22
            }
        }
    }
    return [Sevenzip(), Zstd(compression_level=22), Zpaq(), Rar(), Gzip(), Bzip2(compression_level=9), SubPrePCS(test_instance_params)]
    # return [Bzip2(), SubPrePCS(), SubPrePCS(test_instance_params)]
    # return [SubPrePCS(test_instance_params)]


def benchmark_clis(input_dir, output_dir):
    formatted_now = datetime.now().strftime("%Y%m%d_%H%M%S")
    cli_instances = create_cli_instances()
    results = common.run_commands_in_parallel(cli_instances, input_dir, 10)
    output = os.path.join(output_dir, f'benchmark_results-{formatted_now}.csv')
    common.save_results(results, output)


if __name__ == '__main__':
    input_directory = '../../test_files/logs'
    output_directory = "results"
    benchmark_clis(input_directory, output_directory)

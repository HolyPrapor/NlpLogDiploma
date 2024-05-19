import os
from datetime import datetime

import common
from CLIs.subprepcs import SubPrePCS
from CLIs.bzip import Bzip2


def create_cli_instances():
    test_instance_params = {
        "primary_log_coder": {
            "storage_size": 2
        }
    }
    return [Bzip2(), SubPrePCS(), SubPrePCS(test_instance_params)]


def benchmark_clis(input_dir, output_dir):
    formatted_now = datetime.now().strftime("%Y%m%d_%H%M%S")
    cli_instances = create_cli_instances()
    results = common.run_commands_in_parallel(cli_instances, input_dir)
    output = os.path.join(output_dir, f'benchmark_results-{formatted_now}.csv')
    common.save_results(results, output)


if __name__ == '__main__':
    input_directory = '../../test_files/logs'
    output_directory = "/tmp/subprepcs"
    benchmark_clis(input_directory, output_directory)

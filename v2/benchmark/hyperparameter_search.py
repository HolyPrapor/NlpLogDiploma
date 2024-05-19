import abc
import itertools
import os
from datetime import datetime

import common
from v2.benchmark.CLIs.subprepcs import SubPrePCS


class SingleCell:
    def __init__(self, name, *fields):
        self.name = name
        self.fields = fields

    def __str__(self):
        return f"{self.name}({', '.join(str(f) for f in self.fields)})"


class OneOfCell:
    def __init__(self, name, *contents):
        self.name = name
        self.contents = contents

    def __str__(self):
        return f"{self.name}({', '.join(str(c) for c in self.contents)})"


class ListCell:
    def __init__(self, name, *contents):
        self.name = name
        self.contents = contents

    def __str__(self):
        return f"{self.name}({', '.join(str(c) for c in self.contents)})"


def expand_cell(base, cell):
    if isinstance(cell, SingleCell):
        expanded = [base] # empty dict
        for field in cell.fields: # adding fields one by one
            should_embed = isinstance(field, OneOfCell) or isinstance(field, ListCell)
            new_expanded = []
            for new in expand_cell({}, field): # expanded field (in depth)
                for current in expanded:
                    if should_embed:
                        new_expanded.append({**current, **new})
                    else:
                        new_expanded.append({**current, field.name: new}) # added new field to current
            expanded = new_expanded
        return expanded
    if isinstance(cell, OneOfCell):
        results = []
        for content in cell.contents:
            should_embed = isinstance(content, OneOfCell) or isinstance(content, ListCell)
            expanded = expand_cell({}, content)
            for item in expanded:
                results.append({**base, content.name: item})
        return results
    if isinstance(cell, ListCell):
        results = []
        for content in cell.contents:
            expanded = expand_cell({}, content)
            for item in expanded:
                results.append({**base, cell.name: item})
        return results
    return [cell]


def create_cli_instances():
    """Creates CLI instances for all parameter combinations."""
    grid = SingleCell("grid",
        SingleCell("primary_log_coder",
                   OneOfCell("log_link_coder", SingleCell("residue_log_link_coder")),
                   OneOfCell("storage",
                             SingleCell("greedy_sliding_window_storage"),
                             SingleCell("greedy_move_to_front_storage")),
                   ListCell("storage_size", *[50, 255]),
                   ListCell("min_link_length", *[6, 10])
                   ),
        SingleCell("secondary_log_coder",
                   OneOfCell("secondary_log_coder",
                             # SingleCell("residue_secondary_log_coder"),
                             SingleCell("ppm_secondary_log_coder", ListCell("window_size", *[2, 3, 4]))))
          )

    return [SubPrePCS(params) for params in expand_cell({}, grid)]


def hyperparameter_search(input_dir, output_dir):
    formatted_now = datetime.now().strftime("%Y%m%d_%H%M%S")
    cli_instances = create_cli_instances()
    results = common.run_commands_in_parallel(cli_instances, input_dir, 2)
    output = os.path.join(output_dir, f'hyperparameter_search_results-{formatted_now}.csv')
    common.save_results(results, output)


if __name__ == '__main__':
    input_directory = '../../test_files/logs'
    output_directory = '/tmp/subprepcs'
    hyperparameter_search(input_directory, output_directory)



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
        expanded = [base]  # empty dict
        for field in cell.fields:  # adding fields one by one
            should_embed = isinstance(field, OneOfCell) or isinstance(field, ListCell)
            new_expanded = []
            for new in expand_cell({}, field):  # expanded field (in depth)
                for current in expanded:
                    if should_embed:
                        new_expanded.append({**current, **new})
                    else:
                        new_expanded.append({**current, field.name: new})  # added new field to current
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
    grid = SingleCell(
        "grid",
        SingleCell(
            "primary_log_coder",
            SingleCell("residue_log_link_coder"),
            SingleCell(
                "greedy_move_to_front_2_storage",
                ListCell("static_movement_degree", *[1]),
                ListCell("stable", *[False]),
            ),
            ListCell("storage_size", *[255, 1020]),
            ListCell("min_link_length", *[3, 6, 10]),
            OneOfCell(
                "filter",
                # SingleCell(
                #     "ngram_log_filter",
                #     ListCell("n", *[1]),
                #     ListCell("acceptance_threshold", *[0.55, 0.65])
                # ),
                # SingleCell(
                #     "ngram_log_filter",
                #     ListCell("n", *[2]),
                #     ListCell("acceptance_threshold", *[0.155, 0.165])
                # ),
                SingleCell(
                    "ngram_log_filter",
                    ListCell("n", *[1]),
                    ListCell("acceptance_threshold", *[0.765, 0.775]),
                    ListCell("use_cosine_similarity", *[True]),
                ),
                SingleCell(
                    "ngram_log_filter",
                    ListCell("n", *[2]),
                    ListCell("acceptance_threshold", *[0.27, 0.28]),
                    ListCell("use_cosine_similarity", *[True]),
                ),
            ),
            ListCell("use_optimal_partitioning", *[False]),
        ),
        SingleCell(
            "secondary_log_coder",
            SingleCell("residue_secondary_log_coder"),
        ),
        SingleCell(
            "generic_primary_coder",
            SingleCell(
                "zstd_coder",
                ListCell("compression_level", *[22]),
            ),
        ),
        SingleCell(
            "generic_secondary_coder",
            SingleCell(
                "zstd_coder",
                ListCell("compression_level", *[22]),
            ),
        ),
        SingleCell(
            "generic_markup_coder",
            SingleCell(
                "zstd_coder",
                ListCell("compression_level", *[22]),
            ),
        )
    )

    return [SubPrePCS(params) for params in expand_cell({}, grid)]


def hyperparameter_search(input_dir, output_dir):
    formatted_now = datetime.now().strftime("%Y%m%d_%H%M%S")
    cli_instances = create_cli_instances()
    results = common.run_commands_in_parallel(cli_instances, input_dir, 16)
    output = os.path.join(output_dir, f'hyperparameter_search_results-{formatted_now}.csv')
    common.save_results(results, output)


if __name__ == '__main__':
    input_directory = '../../test_files/logs'
    output_directory = 'results'
    hyperparameter_search(input_directory, output_directory)

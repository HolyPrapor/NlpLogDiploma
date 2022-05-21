import math
from typing import *
import glob
from collections import Counter

def get_stats(template: str) -> Tuple[float, int]:
    total = 0
    counts = Counter()
    for file in glob.glob(template):
        with open(file, mode='rb') as f:
            data = f.read()
            size += len(data)
            counts.update(data)
    entropy = 0
    for _, count in counts:
        p = count / size
        entropy -= p * math.log(p, 256)
    return entropy, total

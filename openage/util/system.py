# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Various OS utilities
"""

import re
from sys import platform

from .math import INF


def free_memory():
    """
    Returns the amount of free bytes of memory.
    On failure, returns +inf.

    >>> free_memory() > 0
    True
    """
    if platform.startswith('linux'):
        pattern = re.compile('^MemAvailable: +([0-9]+) kB\n$')
        with open('/proc/meminfo') as meminfo:
            for line in meminfo:
                match = pattern.match(line)
                if not match:
                    continue
                return 1024 * int(match.group(1))
        return INF

    if platform == "darwin":
        # TODO
        return INF
    if platform == "win32":
        # TODO
        return INF

    # unknown platform
    return INF

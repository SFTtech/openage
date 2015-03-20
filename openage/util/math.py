# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Holds some math constants and helpers
"""

import math

# I tend to use this all the time. I don't get why it's not in math.
INF = float("+inf")

# TAU for life!
TAU = 2 * math.pi
DEGSPERRAD = TAU / 360


def clamp(val, minval, maxval):
    """
    clamps val to be at least minval, and at most maxval.
    """
    return min(maxval, max(minval, val))

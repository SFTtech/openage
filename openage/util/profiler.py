# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Profiling utilities
"""

import cProfile
import pstats
import io


class Profiler:
    """
    A class for quick and easy profiling.
    Usage:
            p = Profiler()
            with p:
                #Call methods that need to be profiled.
            p.report()
    The 'with' statement can be replaced with calls to
    p.enable() and p.disable().
    """

    profile = None
    profile_stats = None
    profile_stream = None

    def __init__(self, oStream=None):
        # oStream can be a file if the profile results want to be saved.
        self.profile = cProfile.Profile()
        self.profile_stream = oStream

    def __enter__(self):
        # begins profiling
        self.enable()

    def __exit__(self, exc_type, exc_value, traceback):
        # Stops profiling
        self.disable()

    def write_report(self, sortby='calls'):
        """Write the profile stats to profile_stream's file."""
        self.profile_stats = pstats.Stats(self.profile, stream=self.profile_stream)
        self.profile_stats.sort_stats(sortby)
        self.profile_stats.print_stats()

    def report(self, sortby='calls'):
        """Return the profile_stats to the console."""
        self.profile_stats = pstats.Stats(self.profile, stream=io.StringIO())
        self.profile_stats.sort_stats(sortby)
        self.profile_stats.print_stats()
        return self.profile_stats.stream.getvalue()

    def enable(self):
        """Begins profiling calls."""
        self.profile.enable()

    def disable(self):
        """Stop profiling calls."""
        self.profile.disable()

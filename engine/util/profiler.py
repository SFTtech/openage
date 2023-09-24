# Copyright 2017-2022 the openage authors. See copying.md for legal info.

"""
Profiling utilities
"""

import cProfile
import io
import pstats
import tracemalloc


class Profiler:
    """
    A class for quick and easy profiling.
    Usage:
        p = Profiler()
        with p:
            # call methods that need to be profiled here
        print(p.report())

    The 'with' statement can be replaced with calls to
    p.enable() and p.disable().
    """

    profile: cProfile.Profile = None
    profile_stats: pstats.Stats = None
    profile_stream = None

    def __init__(self, o_stream=None):
        # o_stream can be a file if the profile results want to be saved.
        self.profile = cProfile.Profile()
        self.profile_stream = o_stream

    def __enter__(self):
        """
        Activate data collection.
        """
        self.enable()

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Stop profiling.
        """
        self.disable()

    def write_report(self, sortby: str = 'calls') -> None:
        """
        Write the profile stats to profile_stream's file.
        """
        self.profile_stats = pstats.Stats(self.profile, stream=self.profile_stream)
        self.profile_stats.sort_stats(sortby)
        self.profile_stats.print_stats()

    def report(self, sortby: str = 'calls'):
        """
        Return the profile_stats to the console.
        """
        self.profile_stats = pstats.Stats(self.profile, stream=io.StringIO())
        self.profile_stats.sort_stats(sortby)
        self.profile_stats.print_stats()
        return self.profile_stats.stream.getvalue()

    def enable(self):
        """
        Begins profiling calls.
        """
        self.profile.enable()

    def disable(self):
        """
        Stop profiling calls.
        """
        self.profile.disable()


class Tracemalloc:
    """
    A class for memory profiling.
    Usage:
        p = Tracemalloc()
        with p:
            # call methods that need to be profiled here
        print(p.report())

    The 'with' statement can be replaced with calls to
    p.enable() and p.disable().
    """

    snapshot0 = None
    snapshot1 = None
    peak = None

    def __init__(self, o_stream=None):
        # o_stream can be a file if the profile results want to be saved.
        self.tracemalloc_stream = o_stream

    def __enter__(self):
        """
        Activate data collection.
        """
        self.enable()

    def __exit__(self, exc_type, exc_value, traceback):
        """
        Stop profiling.
        """
        self.disable()

    def snapshot(self):
        """
        Take a manual snapshot. Up to two snapshots can be saved.
        report() compares the last two snapshots.
        """
        if self.snapshot0 is None:
            self.snapshot0 = tracemalloc.take_snapshot()

        elif self.snapshot1 is None:
            self.snapshot1 = tracemalloc.take_snapshot()

        else:
            # Push back
            self.snapshot0 = self.snapshot1
            self.snapshot1 = tracemalloc.take_snapshot()

    def report(
        self,
        sortby: str = 'lineno',
        cumulative: bool = False,
        limit: int = 100
    ) -> None:
        """
        Return the snapshot statistics to the console.
        """
        if self.snapshot1:
            stats = self.snapshot1.compare_to(self.snapshot0, sortby, cumulative)[:limit]

        else:
            stats = self.snapshot0.statistics(sortby, cumulative)[:limit]

        for stat in stats:
            print(stat)

    def get_peak(self) -> int:
        """
        Return the peak memory consumption.
        """
        if not self.peak:
            return tracemalloc.get_traced_memory()[1]

        return self.peak

    @staticmethod
    def enable() -> None:
        """
        Begins profiling calls.
        """
        tracemalloc.start()

    def disable(self) -> None:
        """
        Stop profiling calls.
        """
        if self.snapshot0 is None:
            self.snapshot()

        self.peak = tracemalloc.get_traced_memory()[1]

        tracemalloc.stop()

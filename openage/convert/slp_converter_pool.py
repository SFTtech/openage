# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Multiprocessing-based SLP-to-texture converter service.
"""

# TODO This is a temporary workaround for the fact that the SLP conversion
#      requires the GIL, and is really slow right now.
#      Ideally, time-intemsive parts of SLP.py should be re-written as
#      optimized nogil Cython functions, entirely removing the need for
#      multiprocessing.

import multiprocessing
import os
import queue

from threading import Lock

from ..log.logging import get_loglevel
from ..log import warn, err
from ..util.system import free_memory

from .slp import SLP
from .texture import Texture


class SLPConverterPool:
    """
    Multiprocessing-based pool of SLP converter processes.
    """
    def __init__(self, palette, jobs=None):
        if jobs is None:
            jobs = os.cpu_count()

        self.palette = palette

        self.fake = (jobs == 1)
        if self.fake:
            # don't actually create the entire multiprocessing thing.
            # self.convert() will just do the conversion directly.
            return

        # Holds the queues for all currently-idle processes.
        # those queues accept slpdata (bytes) objects, and provide
        # Texture objects in return.
        # Note that this is a queue.Queue, not a multiprocessing.Queue.
        self.idle = queue.Queue()

        # guards new job submission so we can "throttle" it.
        self.job_mutex = Lock()

        # Holds tuples of (process, queue) for all processes.
        # Needed for proper termination in close().
        self.processes = []

        # spawn worker processes,
        # each has a queue where data is pushed to the process.
        for _ in range(jobs):
            inqueue = multiprocessing.Queue()
            outqueue = multiprocessing.Queue()

            process = multiprocessing.Process(
                target=converter_process,
                args=(inqueue, outqueue)
            )

            process.start()
            self.processes.append((process, inqueue))

            # send initial configuration to process
            inqueue.put(get_loglevel())
            inqueue.put(palette)

            self.idle.put((inqueue, outqueue))

    def close(self):
        """
        Closes the converter pool, quitting all the processes.
        """
        if self.fake:
            return

        for process, inqueue in self.processes:
            inqueue.put(StopIteration)
            process.join()

    def convert(self, slpdata, custom_cutter=None):
        """
        Submits slpdata to one of the converter processes, and returns
        a Texture object (or throws an Exception).
        """
        if self.fake:
            # convert right here, without entering the thread.
            return Texture(SLP(slpdata), self.palette, custom_cutter)

        if free_memory() < 2**30:
            # TODO print the warn only once
            warn("Low on memory; disabling parallel SLP conversion")

            # acquire job_mutex in order to block any concurrent activity until
            # this job is done.
            with self.job_mutex:  # pylint: disable=not-context-manager
                return Texture(SLP(slpdata), self.palette, custom_cutter)

        # get the data queue for an idle worker process
        inqueue, outqueue = self.idle.get()

        # restrict new job submission by free memory (see above)
        with self.job_mutex:  # pylint: disable=not-context-manager
            inqueue.put((slpdata, custom_cutter))

        result = outqueue.get()

        # the process is idle again.
        self.idle.put((inqueue, outqueue))

        if isinstance(result, BaseException):
            err("exception in worker process: %s" % result)
            raise result
        else:
            return result

    def __enter__(self):
        return self

    def __exit__(self, exctype, value, traceback):
        del exctype, value, traceback  # unused
        self.close()


def converter_process(inqueue, outqueue):
    """
    This is the function that runs inside each individual process.
    """
    import sys

    from ..log.logging import set_loglevel

    # prevent writes to sys.stdout
    sys.stdout.write = sys.stderr.write

    # receive initial configuration
    loglevel = inqueue.get()
    palette = inqueue.get()

    # set the loglevel
    set_loglevel(loglevel)

    # loop
    while True:
        work_item = inqueue.get()
        if work_item == StopIteration:
            return

        slpdata, custom_cutter = work_item

        try:
            texture = Texture(SLP(slpdata), palette, custom_cutter)
            outqueue.put(texture)
        except BaseException as exc:
            import traceback
            traceback.print_exc()

            outqueue.put(exc)

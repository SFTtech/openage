# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Threading utilities.
"""

from concurrent.futures import ThreadPoolExecutor
from enum import Enum
import itertools
import os
from queue import Queue


def concurrent_chain(generators, jobs=None):
    """
    Similar to itertools.chain(), but runs the individual generators in a
    thread pool. The resulting items may be out of order accordingly.

    When one generator raises an exception, all other currently-running
    generators are stopped (they may run until their next 'yield' statement).
    The exception is then raised.
    """
    if jobs is None:
        jobs = os.cpu_count()

    if jobs == 1:
        # we don't need to do all that threading stuff;
        # let's just behave _precisely_ like itertools.chain.
        for generator in generators:
            yield from generator
        return

    queue = ClosableQueue()
    running_generator_count = 0

    with ThreadPoolExecutor(jobs) as pool:
        for generator in generators:
            pool.submit(generator_to_queue, generator, queue)
            running_generator_count += 1

        while running_generator_count > 0:
            event_type, value = queue.get()

            if event_type == GeneratorEvent.VALUE:
                yield value
            elif event_type == GeneratorEvent.EXCEPTION:
                queue.close("Exception in different generator")
                raise value
            elif event_type == GeneratorEvent.STOP_ITERATION:
                running_generator_count -= 1


class ClosableQueue(Queue):
    """
    For use in concurrent_chain.

    Behaves like Queue until close() has been called.
    After that, any call to put() raises RuntimeError.
    """
    def __init__(self):
        super().__init__()
        self.closed = False
        self.close_reason = None

    def put(self, item, block=True, timeout=None):
        self.raise_if_closed()
        super().put(item, block, timeout)

    def close(self, reason=None):
        """
        Any subsequent calls to put() or raise_if_closed()
        will raise RuntimeError(reason).
        """
        with self.mutex:
            self.closed = True
            self.close_reason = reason

    def raise_if_closed(self):
        """
        Raises RuntimeError(reason) if the queue has been closed.
        Returns None elsewise.
        """
        with self.mutex:
            if self.closed:
                raise RuntimeError(self.close_reason)


class GeneratorEvent(Enum):
    """
    For use by concurrent_chain.
    Represents any event that a generator may cause.
    """
    VALUE = 0
    EXCEPTION = 1
    STOP_ITERATION = 2


def generator_to_queue(generator, queue):
    """
    For use by concurrent_chain.
    Appends all of the generator's events to the queue,
    as tuples of (event type, value).
    """
    try:
        queue.raise_if_closed()
        for item in generator:
            queue.put((GeneratorEvent.VALUE, item))

        queue.put((GeneratorEvent.STOP_ITERATION, None))
    except BaseException as exc:
        queue.put((GeneratorEvent.EXCEPTION, exc))


def test_concurrent_chain():
    """ Tests concurrent_chain """
    from ..testing.testing import assert_value, assert_raises, result

    def errorgen():
        """ Test generator that raises an exception """
        yield "errorgen"
        raise ValueError()

    assert_value(list(concurrent_chain([], 2)), [])

    assert_value(list(concurrent_chain([range(10)], 2)), list(range(10)))

    assert_value(
        sorted(list(concurrent_chain([range(10), range(20)], 2))),
        sorted(list(itertools.chain(range(10), range(20))))
    )

    chain = concurrent_chain([range(10), range(20), errorgen(), range(30)], 2)
    with assert_raises(ValueError):
        result(list(chain))

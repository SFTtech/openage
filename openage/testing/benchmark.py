# Copyright 2017-2017 the openage authors. See copying.md for legal info.

""" Benchmarking tools for the tests. """

from timeit import timeit
from sys import stdout
from time import sleep


def benchmark_test_function():
    """ Simple function to call in for benchmarking. """
    sleep(0.1)


def benchmark(func):
    """
    Benchmark the given function. Repeated execution helps to give a maximum to
    the consumed time, until one iteration takes more than 5s, summed up 10s.
    """

    result = [(0, 1)]
    number = 1
    total = [0, 0]
    str_row_format = "{:10} {:12}  {:11}"
    row_format1 = "{:10} "
    row_format2 = "{:11.8f}s  {:10.8f}s"
    row_format = row_format1 + row_format2

    print(str_row_format.format("Iterations", "Total time", "Average time per execution"))
    while number < 4 or result[-1][0] < 5 and number < 65537:
        print(row_format1.format(number), end="")
        stdout.flush()

        time = timeit(stmt=func, number=number)
        result.append((time, number))
        print(row_format2.format(time, time / number))
        total[0] += number
        total[1] += time
        stdout.flush()
        number *= 2

    del result[0]

    print()
    print("Benchmark Results: ")
    print("------------------")
    print(str_row_format.format("Iterations", "Total time", "Average time per execution"))
    print(row_format.format(total[0], total[1], total[1] / total[0]))

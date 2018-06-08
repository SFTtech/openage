# Copyright 2015-2018 the openage authors. See copying.md for legal info.

""" Testing code for the openage.log package. """

import argparse
from multiprocessing.pool import ThreadPool

from . import spam, dbg, info, warn, err, crit, \
    set_loglevel, ENV_VERBOSITY, verbosity_to_level


def demo(args):
    """ Demonstrates the Python logging facility. """

    cli = argparse.ArgumentParser()
    cli.add_argument("--verbose", "-v", action='count', default=ENV_VERBOSITY)
    cli.add_argument("--quiet", "-q", action='count', default=0)
    args = cli.parse_args(args)

    level = verbosity_to_level(args.verbose - args.quiet)

    info("new log level: %s", level)
    old_level = set_loglevel(level)
    info("old level was: %s", old_level)

    info("printing some messages with different log levels")

    spam("rofl")
    dbg("wtf?")
    info("foo")
    warn("WARNING!!!!")
    err("that didn't go so well")
    crit("pretty critical, huh?")

    info("restoring old loglevel")

    set_loglevel(old_level)

    info("old loglevel restored")
    info("running some threaded stuff")

    pool = ThreadPool()
    for i in range(8):
        pool.apply_async(info, ("async message #%s", i))
    pool.close()
    pool.join()

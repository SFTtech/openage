# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Python logging.

Log messages get redirected to the CPP logging system if the library has been
loaded.
"""

from enum import Enum, unique
from inspect import currentframe
from os import environ
from threading import Lock

from ..util.math import clamp


@unique
class Level(Enum):
    """
    Log levels with color codes, corresponding to those defined in cpp/log/level.h.
    """

    def __init__(self, numeric, colorcode=""):
        self.numeric = numeric
        self.colorcode = colorcode

    MIN = -3
    spam = -2
    dbg = -1
    info = 0
    warn = (1, "33")
    err = (2, "31;1")
    crit = (3, "31;1;47")
    MAX = 4


def level_colorcode(lvl):
    """ returns the same color codes as in libopenage/log/level.cpp. """

    return lvl.colorcode


Level.current = Level.MIN
# protects the global objects.
LOCK = Lock()


def log(lvl, msg, stackframes=1):
    """
    Logs msg with the given level.
    Goes back the given amount of stackframes for message metadata.
    """
    if lvl not in Level:
        raise Exception("log expects a Level object for its first argument.")

    msg = str(msg)

    cpp_lvl = getattr(lvl, 'cpp', None)

    if cpp_lvl is None:
        # the C++ interface is uninitialized.
        with LOCK:  # pylint: disable=not-context-manager
            if lvl.value >= Level.current.value:
                print("\x1b[" + level_colorcode(lvl) + "m" +
                      lvl.name.upper().rjust(4) + "\x1b[m " +
                      msg)

    else:
        from .log_cpp import log as log_cpp

        frame = currentframe()
        for _ in range(stackframes):
            frame = frame.f_back

        log_cpp(cpp_lvl, msg, frame)


def set_loglevel(lvl):
    """
    Sets the global loglevel (both in Python and in C++).

    Returns the old loglevel.
    """
    if lvl not in Level:
        raise Exception("set_level expects a Level object argument.")

    with LOCK:  # pylint: disable=not-context-manager
        previous = Level.current
        Level.current = lvl

        cpp_lvl = getattr(lvl, 'cpp', None)
        if cpp_lvl is not None:
            # the C++ interface is initialized.
            from .log_cpp import set_level
            set_level(cpp_lvl)

        return previous


def get_loglevel():
    """
    Returns the global loglevel, as stored in Python.
    """
    with LOCK:  # pylint: disable=not-context-manager
        return Level.current


def verbosity_to_level(verbosity):
    """
    Translates an integer verbosity to a log level.
    """
    return Level(clamp(-verbosity, Level.MIN.value, Level.MAX.value))


def env_verbosity():
    """
    Tries to retrieve verbosity from the VERBOSITY environment variable.
    """
    try:
        val = environ['VERBOSE']
    except KeyError:
        return 0

    if val.lower() in {'y', 'yes', 'true'}:
        return 1
    elif val.lower() in {'max'}:
        return 3
    else:
        try:
            return int(val)
        except ValueError:
            return 0


ENV_VERBOSITY = env_verbosity()

# Copyright 2014-2018 the openage authors. See copying.md for legal info.

"""
Python logging.

Log messages get redirected to the CPP logging system if the library has been
loaded.
"""

import logging
from logging import Handler
from os import environ

from ..util.math import clamp

PYTHON_TO_CPP_LOG_LEVEL = {}


class CppHandler(Handler):
    """
    CppHandler calls into the CPP logging system if the library has been loaded.
    """
    def __init__(self):
        super().__init__()

    def setLevel(self, level):
        """ sets the log level """
        cpp_level = PYTHON_TO_CPP_LOG_LEVEL.get(level, None)
        if cpp_level is not None:
            # the C++ interface is initialized
            super().setLevel(level)
            from .log_cpp import set_level as set_level_cpp
            set_level_cpp(cpp_level)

    def emit(self, record):
        """ logs a message """
        cpp_level = PYTHON_TO_CPP_LOG_LEVEL.get(record.levelno, None)
        if cpp_level is None:
            # the C++ interface is uninitialized
            print("\x1b[" + level_colorcode(record.levelno) + "m" +
                  record.levelname.rjust(4) + "\x1b[m " +
                  record.getMessage())
        else:
            from .log_cpp import log as log_cpp
            log_cpp(cpp_level, record.getMessage(), record.filename, record.funcName, record.lineno)


CPP_HANDLER = CppHandler()


def level_colorcode(lvl):
    """ returns the same color codes as in libopenage/log/level.cpp. """

    colorcodes = {
        logging.WARNING: "33",
        logging.ERROR: "31;1",
        logging.CRITICAL: "31;1;47",
    }
    return colorcodes.get(lvl, "")


SPAM = 5


def _spam(self, msg, *args, **kwargs):
    """ Log 'msg % args' with severity 'SPAM'. """
    if self.isEnabledFor(SPAM):
        self._log(SPAM, msg, args, **kwargs)  # pylint: disable=W0212


def setup_logging():
    """ setup the logging system """
    logging.Logger.spam = _spam

    # do not overwrite any of the predefined levels
    # https://docs.python.org/3/library/logging.html#logging-levels
    logging.addLevelName(1, "MIN")
    logging.addLevelName(SPAM, "SPAM")
    logging.addLevelName(51, "MAX")

    root = logging.getLogger()
    root.addHandler(CPP_HANDLER)
    logging.spam = root.spam


def set_loglevel(level):
    """ sets the log level """
    old_level = get_loglevel()
    logging.root.setLevel(level)
    CPP_HANDLER.setLevel(level)
    return old_level


def get_loglevel():
    """ gets the log level """
    return logging.root.level


def spam(msg, *args, **kwargs):
    """ spam message """
    logging.spam(msg, *args, **kwargs)


def dbg(msg, *args, **kwargs):
    """ debug message """
    logging.debug(msg, *args, **kwargs)


def info(msg, *args, **kwargs):
    """ info message """
    logging.info(msg, *args, **kwargs)


def warn(msg, *args, **kwargs):
    """ warning message """
    logging.warning(msg, *args, **kwargs)


def err(msg, *args, **kwargs):
    """ error message """
    logging.error(msg, *args, **kwargs)


def crit(msg, *args, **kwargs):
    """ critical error message """
    logging.critical(msg, *args, **kwargs)


def verbosity_to_level(verbosity):
    """
    Translates an integer verbosity to a log level.
    """
    levels = [logging.getLevelName("MIN"),
              logging.getLevelName("SPAM"),
              logging.DEBUG,
              logging.INFO,
              logging.WARNING,
              logging.ERROR,
              logging.CRITICAL,
              logging.getLevelName("MAX")]
    # return INFO when verbosity is 0
    return levels[clamp(-verbosity + 3, 0, len(levels) - 1)]


def env_verbosity():
    """
    Tries to retrieve verbosity from the VERBOSITY environment variable.
    """
    val = environ.get('VERBOSE', '0')

    if val.lower() in {'y', 'yes', 'true'}:
        return logging.WARNING
    elif val.lower() in {'max'}:
        return logging.CRITICAL
    else:
        try:
            return int(val)
        except ValueError:
            return logging.INFO


ENV_VERBOSITY = env_verbosity()

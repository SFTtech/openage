# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
openage python logging system.
"""

from .logging import log, Level


def spam(msg):
    """ spam message """
    log(Level.spam, msg, 2)


def dbg(msg):
    """ debug message """
    log(Level.dbg, msg, 2)


def info(msg):
    """ info message """
    log(Level.info, msg, 2)


def warn(msg):
    """ warning message """
    log(Level.warn, msg, 2)


def err(msg):
    """ error message """
    log(Level.err, msg, 2)


def crit(msg):
    """ critical error message """
    log(Level.crit, msg, 2)

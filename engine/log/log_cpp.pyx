# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Translates Python log messages to C++ log messages.
"""

from libcpp.memory cimport unique_ptr

from libopenage.log.message cimport message
from libopenage.log.level cimport (
    level,

    MIN,
    spam,
    dbg,
    info,
    warn,
    err,
    crit,
    MAX
)

from libopenage.log.named_logsource cimport NamedLogSource
from libopenage.log.log cimport set_level as cpp_set_level

import logging
from inspect import getframeinfo
from ..log import get_loglevel, PYTHON_TO_CPP_LOG_LEVEL


cdef class CPPLevel:
    """
    Holds a C++ 'level' object.
    """
    cdef level value

    cdef level get(self):
        return self.value

    @staticmethod
    cdef wrap(level lvl):
        cdef CPPLevel result = CPPLevel()
        result.value = lvl
        return result

cdef unique_ptr[NamedLogSource] PY_LOGSOURCE


def enable_log_translation():
    """
    To be called from setup.setup after the interface has been
    initialized.
    """
    PY_LOGSOURCE.reset(new NamedLogSource(b"py"))

    PYTHON_TO_CPP_LOG_LEVEL[logging.getLevelName("MIN")] = CPPLevel.wrap(MIN)
    PYTHON_TO_CPP_LOG_LEVEL[logging.getLevelName("SPAM")] = CPPLevel.wrap(spam)
    PYTHON_TO_CPP_LOG_LEVEL[logging.DEBUG] = CPPLevel.wrap(dbg)
    PYTHON_TO_CPP_LOG_LEVEL[logging.INFO] = CPPLevel.wrap(info)
    PYTHON_TO_CPP_LOG_LEVEL[logging.WARNING] = CPPLevel.wrap(warn)
    PYTHON_TO_CPP_LOG_LEVEL[logging.ERROR] = CPPLevel.wrap(err)
    PYTHON_TO_CPP_LOG_LEVEL[logging.CRITICAL] = CPPLevel.wrap(crit)
    PYTHON_TO_CPP_LOG_LEVEL[logging.getLevelName("MAX")] = CPPLevel.wrap(MAX)

    set_level(PYTHON_TO_CPP_LOG_LEVEL[get_loglevel()])


def log(CPPLevel lvl, str msg, filename, function, lineno):
    """
    Forwards the message to C++.
    """
    cdef message cpp_msg

    cpp_msg.init_with_metadata_copy(
        filename.encode(),
        function.encode())

    cpp_msg.lineno = lineno
    cpp_msg.lvl = lvl.value

    cpp_msg.text = msg.encode()

    PY_LOGSOURCE.get().log(cpp_msg)


def set_level(CPPLevel lvl):
    cpp_set_level(lvl.get())

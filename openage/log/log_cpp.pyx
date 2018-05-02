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

from inspect import getframeinfo

from .logging import Level


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

    Level.MIN.cpp = CPPLevel.wrap(MIN)
    Level.spam.cpp = CPPLevel.wrap(spam)
    Level.dbg.cpp = CPPLevel.wrap(dbg)
    Level.info.cpp = CPPLevel.wrap(info)
    Level.warn.cpp = CPPLevel.wrap(warn)
    Level.err.cpp = CPPLevel.wrap(err)
    Level.crit.cpp = CPPLevel.wrap(crit)
    Level.MAX.cpp = CPPLevel.wrap(MAX)

    set_level(Level.current.cpp)


def log(CPPLevel lvl, str msg, frame):
    """
    Forwards the message to C++.
    """
    frameinfo = getframeinfo(frame, 0)

    cdef message cpp_msg

    cpp_msg.init_with_metadata_copy(
        frameinfo.filename.encode(),
        frameinfo.function.encode())

    cpp_msg.lineno = frameinfo.lineno
    cpp_msg.lvl = lvl.value

    cpp_msg.text = msg.encode()

    PY_LOGSOURCE.get().log(cpp_msg)


def set_level(CPPLevel lvl):
    cpp_set_level(lvl.get())

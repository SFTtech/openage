# Copyright 2026-2026 the openage authors. See copying.md for legal info.
# cython: c_string_type=unicode, c_string_encoding=utf8

"""
Tests for accessing Python fslike objects from C++ through cpp.pyx.
"""

import os
from tempfile import TemporaryDirectory

from cpython.ref cimport PyObject

from libopenage.pyinterface.pyobject cimport PyObj
from libopenage.util.path cimport Path as Path_cpp

from ...testing.testing import assert_value
from .directory import Directory
from .union import Union
from .wrapper import WriteBlocker


def test():
    """
    Query Python fslikes through C++ paths, like the engine does with the
    root union it gets from Python.
    """
    cdef Path_cpp converted
    cdef Path_cpp modpack_file
    cdef Path_cpp blocked_file

    with TemporaryDirectory() as tmp:
        os.makedirs(os.path.join(tmp, "converted"))
        native_file = os.path.join(tmp, "converted", "modpack.toml")
        with open(native_file, "w", encoding="utf-8"):
            pass

        union = Union()
        union.root["assets"].mount(Directory(tmp).root)

        converted = Path_cpp(["assets", "converted"], PyObj(<PyObject *>union))
        assert_value(converted.is_dir(), True)
        assert_value(converted.is_file(), False)
        assert_value(list(converted.list()), ["modpack.toml"])

        # resolving through the union yields a Directory, which C++ converts
        # to its native fslike implementation
        modpack_file = Path_cpp(["assets", "converted", "modpack.toml"], PyObj(<PyObject *>union))
        assert_value(modpack_file.is_file(), True)
        assert_value(modpack_file.resolve_native_path("r"), native_file)

        # the outer wrapper resolves to the inner one, which stays a Python
        # fslike, so C++ asks Python for the native path
        blocker = WriteBlocker(WriteBlocker(Directory(tmp).root).root)
        blocked_file = Path_cpp(["converted", "modpack.toml"], PyObj(<PyObject *>blocker))
        assert_value(blocked_file.resolve_native_path("r"), native_file)

# Copyright 2016-2017 the openage authors. See copying.md for legal info.

from os.path import expanduser, expandvars
from pathlib import Path

from libcpp.string cimport string

from libopenage.cvar.cvar cimport (
    CVarManager,
    pyx_load_config_file
)
from libopenage.util.path cimport Path as Path
from openage.util.fslike.cpp cimport cpppath_to_pypath

from .config_file import load_config_file
from .. import default_dirs


# TODO: turn this into a config and user profile system.


cdef void cy_load_config_file(CVarManager *manager, const Path &path) except * with gil:
    """
    Relay the call to load values from a config file
    into the configuration manager.

    Effectively glues together the call from C++ to Python.
    """

    def set_func(key, val):
        manager.set(key.encode(), val.encode())

    py_path = cpppath_to_pypath(path)
    load_config_file(py_path, set_func)


def setup():
    pyx_load_config_file.bind0(cy_load_config_file)

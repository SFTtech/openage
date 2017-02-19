# Copyright 2016-2017 the openage authors. See copying.md for legal info.

from os.path import expanduser, expandvars
from pathlib import Path

from libcpp.string cimport string

from libopenage.cvar.cvar cimport (
    CVarManager,
    pyx_load_config_file,
    pyx_config_file_set_option
)
from libopenage.util.path cimport Path as Path
from openage.util.fslike.cpp cimport cpppath_to_pypath

from .config_file import (
    load_config_file,
    config_file_set_option,
    config_file_append_option)
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


cdef void cy_config_file_set_option(const Path &path, string name,
                                    string value) except * with gil:
    """
    Set a value in a configuration file.
    """

    py_path = cpppath_to_pypath(path)

    if not config_file_set_option(py_path,
                                  name.decode('UTF-8'),
                                  value.decode('UTF-8')):
        config_file_append_option(py_path,
                                  name.decode('UTF-8'),
                                  value.decode('UTF-8'))


def setup():
    pyx_load_config_file.bind0(cy_load_config_file)
    pyx_config_file_set_option.bind0(cy_config_file_set_option)

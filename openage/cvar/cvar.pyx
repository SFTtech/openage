# Copyright 2016-2017 the openage authors. See copying.md for legal info.

from os.path import expanduser, expandvars
from pathlib import Path

from libcpp.string cimport string

from libopenage.cvar.cvar cimport (
    CVarManager,
    load_config_file as load_config_file_cpp,
    find_main_config_file as find_main_config_file_cpp)

from .config_file import load_config_file as load_config_py
from .. import default_dirs


# TODO: turn this into a config and user profile system.


cdef void load_config_file(string path,
                           CVarManager *manager) except * with gil:
    """
    Relay the call to load values from a config file
    into the configuration manager.

    Effectively glues together the call from C++ to Python.
    """

    load_config_py(Path(path.decode('UTF-8')),
                   lambda x, y: manager.set(x.encode('UTF-8'),
                                            y.encode('UTF-8')))


cdef string find_main_config_file() except * with gil:
    """
    Locates the main configuration file by name in some searchpaths.
    """

    cfg_file = "keybinds.oac"

    # TODO: create the config file there if the default one is modified
    #       this is probably a job for the profile creation though.
    home_cfg = default_dirs.get_dir("config_home")/"openage"/cfg_file
    if home_cfg.is_file():
        return str(home_cfg.resolve()).encode("UTF-8")

    # TODO: config_dirs can be multiple paths!
    global_cfg = default_dirs.get_dir("config_dirs")/"openage"/cfg_file
    if global_cfg.is_file():
        return str(global_cfg.resolve()).encode("UTF-8")

    # the default config file is in the development repository.
    # TODO: default for some installed version if in non-dev mode.
    return str(Path.cwd()/"cfg"/cfg_file).encode("UTF-8")


def setup():
    load_config_file_cpp.bind0(load_config_file)
    find_main_config_file_cpp.bind0(find_main_config_file)

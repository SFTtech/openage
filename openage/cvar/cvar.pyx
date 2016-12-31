# Copyright 2016-2016 the openage authors. See copying.md for legal info.

import platform

from libcpp.string cimport string

from libopenage.cvar.cvar cimport (
    CVarManager,
    load_config_file as load_config_file_cpp,
    find_main_config_file as find_main_config_file_cpp)

from .config_file import load_config_file as load_config_py

from os.path import expanduser, expandvars
from pathlib import Path


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
    system = platform.system()

    if system == "Windows":
        # TODO: verify that windows users really store data there
        #       and not just in C:\openage\
        home_expandable = "%APPDATA%"
        global_expandable = "%ALLUSERSPROFILE%"
    else:
        home_expandable = "~/.config"
        global_expandable = "/etc"

    home_cfg = Path(expanduser(home_expandable))/cfg_file
    if home_cfg.is_file():
        return str(home_cfg.resolve()).encode("UTF-8")

    global_cfg = Path(expandvars(global_expandable))/cfg_file
    if global_cfg.is_file():
        return str(global_cfg.resolve()).encode("UTF-8")

    # the default config file is in the development repository.
    # TODO: default for some installed version if in non-dev mode.
    return str(Path.cwd()/"cfg"/cfg_file).encode("UTF-8")


def setup():
    load_config_file_cpp.bind0(load_config_file)
    find_main_config_file_cpp.bind0(find_main_config_file)

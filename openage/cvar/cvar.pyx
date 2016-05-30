# Copyright 2016-2016 the openage authors. See copying.md for legal info.

from libcpp.string cimport string

from libopenage.cvar.cvar cimport CVarManager

from .config_file import load_config_file as load_config

from os.path import expanduser, expandvars
from pathlib import Path
import platform

cdef void load_config_file(string path, CVarManager *manager) except * with gil:
    load_config(Path(path.decode('UTF-8')), lambda x,y : manager[0].set(x.encode('UTF-8'),y.encode('UTF-8')))

cdef string find_main_config_file() except * with gil:
    cfg_file = "keybinds.oac"
    system = platform.system()

    if system == "Windows":
        home_expandable = "%APPDATA%"
        global_expandable = "%ALLUSERSPROFILE%"
    else:
        home_expandable = "~/.config"
        global_expandable = "/etc"

    home_cfg = Path(expanduser(home_expandable))/cfg_file

    if home_cfg.is_file():
        return str(home_cfg.resolve()).encode("UTF-8")

    global_cfg = Path(expandvars(global_expandable)) / cfg_file
    if global_cfg.is_file():
        return str(global_cfg.resolve()).encode("UTF-8")

    return str(Path.cwd()/"cfg"/cfg_file).encode("UTF-8")

def setup():
    CVarManager.get().load_config_file.bind0(load_config_file)
    CVarManager.get().find_main_config_file.bind0(find_main_config_file)

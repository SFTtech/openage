# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""

Code for locating the game assets.

All access to game assets should happen through objects obtained from get().
"""

import os
import pathlib
import sys

# TODO: use os.pathsep for multipath variables

# Linux-specific dirs according to the freedesktop basedir standard:
# https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
#
# concretely:
# $XDG_CONFIG_HOME = $HOME/.config
# $XDG_DATA_HOME = $HOME/.local/share
# $XDG_DATA_DIRS = /usr/local/share/:/usr/share/
# $XDG_CONFIG_DIRS = /etc/xdg
# $XDG_CACHE_HOME = $HOME/.cache
# $XDG_RUNTIME_DIR = /run/user/$UID
#
LINUX_DIRS = {
    "config_home": ("XDG_CONFIG_HOME", ("{HOME}/.config", {"HOME"})),
    "data_home": ("XDG_DATA_HOME", ("{HOME}/.local/share", {"HOME"})),
    "data_dirs": ("XDG_DATA_DIRS", ("/usr/local/share/:/usr/share/", {})),
    "config_dirs": ("XDG_CONFIG_DIRS", ("/etc/xdg", {})),
    "cache_home": ("XDG_CACHE_HOME", ("{HOME}/.cache", {"HOME"})),
    "runtime_dir": ("XDG_RUNTIME_DIR", ("/run/user/$UID")),
}


# Windows-specific paths
WINDOWS_DIRS = {
    "data_home": ("APPDATA", (False, None)),
    "config_dirs": ("ALLUSERSPROFILE", (False, None)),
    # TODO: other windows paths
}


def get_dir(which):
    """
    Returns directories used for data and config storage.
    returns pathlib.Path
    """

    platform_table = None

    if sys.platform.startswith("linux"):
        platform_table = LINUX_DIRS

    elif sys.platform.startswith("darwin"):
        raise Exception("macOS not really supported")

    elif sys.platform.startswith("win32"):
        platform_table = WINDOWS_DIRS
        raise Exception("windows not yet supported")

    else:
        raise Exception("unsupported platform: '%s'" % sys.platform)

    if which not in platform_table:
        raise ValueError("unknown directory requested: '{}'".format(which))

    # fetch the directory template
    env_var, (default_template, required_envs) = platform_table[which]

    # then create the result from the environment
    env_val = os.environ.get(env_var)

    if env_val:
        path = env_val

    elif default_template:
        env_vars = {var: os.environ.get(var) for var in required_envs}
        if not all(env_vars.values()):
            raise Exception("could not reconstruct {}, "
                            "missing env variables: '{}'".format(
                                which,
                                [var for (var, val) in env_var.items()
                                 if val is None].join(", ")
                            ))

        path = default_template.format(**env_vars)

    else:
        raise Exception("could not find '{}' in environment".format(which))

    return pathlib.Path(path)

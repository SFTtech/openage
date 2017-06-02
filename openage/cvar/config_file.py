# Copyright 2016-2017 the openage authors. See copying.md for legal info.

"""
Load and save the configuration : file <-> console var system
"""

from ..log import info


def load_config_file(path, set_cvar_func, loaded_files=None):
    """
    Load a config file, with possible subfile, into the cvar system.

    set_cvar is a function that accepts (key, value) to actually
    add the data.
    """

    if not loaded_files:
        loaded_files = set()

    if not path.is_file():
        info("config file %s not found." % path)
        return

    # file is already loaded?
    # the repr(path) is pretty hacky but does its job.
    # better solution would be to implement __hash__
    if repr(path) in loaded_files:
        return

    info("loading config file %s..." % path)

    loaded_files.add(repr(path))

    with path.open() as config:
        for line in config:
            print("Reading config line: %s" % line)
            lstrip = line.lstrip()
            if not lstrip or lstrip.startswith("#"):
                continue

            strip = lstrip.rstrip()
            split = strip.split()

            if split[0] == "set" and len(split) >= 3:
                set_cvar_func(split[1], " ".join(split[2:]))

            elif split[0] == "load" and len(split) >= 2:
                for sub_path in split[1:]:
                    new_path = path.parent / sub_path
                    load_config_file(new_path, set_cvar_func, loaded_files)

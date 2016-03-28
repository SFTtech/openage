# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Load and save the configuration : file <-> console var system
"""


def load_config_file(path, set_cvar, loaded=None):
    """
    Load a config file, with possible subfile, into the cvar system.
    """
    if not loaded:
        loaded = {}
    if not path.is_file() or path in loaded:
        return

    loaded[path] = True
    with path.open() as config:
        for line in config:
            lstrip = line.lstrip()
            if not lstrip or lstrip.startswith("#"):
                continue

            strip = lstrip.rstrip()
            split = strip.split()

            if split[0] == "set" and len(split) >= 3:
                set_cvar(split[1], " ".join(split[2:]))
            elif split[0] == "load" and len(split) >= 2:
                for sub_path in split[1:]:
                    new_path = path.parent / sub_path
                    load_config_file(new_path, set_cvar, loaded)

# Copyright 2016-2017 the openage authors. See copying.md for legal info.

"""
Load and save the configuration : file <-> console var system
"""

import os
import tempfile
import shutil
from pathlib import Path

from ..log import info, dbg


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


def config_file_set_existing_option(path, name, value, lineno):
    """
    Set a value in a configuration file if the value exists already.
    """

    temp_dir = tempfile.mkdtemp()
    temp_path = Path(temp_dir, 'tmp_openage_cvar')

    dest = temp_path.open("w")

    try:
        with path.open() as config:
            for _ in range(0, lineno):
                line = config.readline()

                if not line:
                    return False

                dest.write(line)

            line = config.readline()

            if not line:
                return False

            lstrip = line.lstrip()
            if not lstrip or lstrip.startswith("#"):
                return False

            strip = lstrip.rstrip()
            split = strip.split()

            if split[0] == "set" and len(split) >= 3 and split[1] == name:
                dest.write(str.join("", ("set ", name, " ", value, "\n")))
            else:
                return False

            for line in config:
                dest.write(line)

        dest.close()

        shutil.copy2(str(temp_path), str(path))

        dbg("saving cvar %s: '%s'" % (name, value))

        return True

    finally:
        os.remove(str(temp_path))
        os.rmdir(temp_dir)

    return False


def config_file_set_option(path, name, value):
    """
    Set a value in a configuration file, with possible subfile.
    """

    if not path.is_file():
        return False

    lineno = -1

    with path.open() as config:
        for line in config:
            lineno += 1

            if not line:
                break

            lstrip = line.lstrip()
            if not lstrip or lstrip.startswith("#"):
                continue

            strip = lstrip.rstrip()
            split = strip.split()

            if split[0] == "set" and len(split) >= 3 and split[1] == name:
                temp_dir = tempfile.mkdtemp()
                temp_path = Path(temp_dir, 'tmp_openage_cvar')

                shutil.copy2(str(path), str(temp_path))

                success = False

                try:
                    success = config_file_set_existing_option(temp_path, name, value, lineno)
                    shutil.copy2(str(temp_path), str(path))
                finally:
                    os.remove(str(temp_path))
                    os.rmdir(temp_dir)

                if success:
                    return True

            elif split[0] == "load" and len(split) >= 2:
                for sub_path in split[1:]:
                    new_path = path.parent / sub_path
                    if config_file_set_option(new_path, name, value):
                        return True

    return False


def config_file_append_option(path, name, value):
    """
    Append a value to a configuration file.
    """

    if not path.is_file():
        return

    dbg("adding cvar %s: '%s'" % (name, value))

    with path.open("a+") as config:
        config.write(str.join("", ("set ", name, " ", value, "\n")))

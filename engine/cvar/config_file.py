# Copyright 2016-2022 the openage authors. See copying.md for legal info.

"""
Load and save the configuration : file <-> console var system
"""

from __future__ import annotations
import typing

from ..log import info, spam


if typing.TYPE_CHECKING:
    from engine.util.fslike.path import Path


def load_config_file(path: Path, set_cvar_func: typing.Callable, loaded_files: set = None) -> None:
    """
    Load a config file, with possible subfile, into the cvar system.

    set_cvar is a function that accepts (key, value) to actually
    add the data.
    """

    if not loaded_files:
        loaded_files = set()

    if not path.is_file():
        info(f"config file {path} not found.")
        return

    # file is already loaded?
    # the repr(path) is pretty hacky but does its job.
    # better solution would be to implement __hash__
    if repr(path) in loaded_files:
        return

    info(f"loading config file {path}...")

    loaded_files.add(repr(path))

    with path.open() as config:
        for line in config:
            spam(f"Reading config line: {line}")
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

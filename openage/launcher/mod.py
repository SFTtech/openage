# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Parser for modpack information to be used by the launcher
"""

import toml


class ModInfo:
    """
    Parses and exposes info about a mod
    """
    def __init__(self, info_str, path=None): 
        self.info = toml.loads(info_str)
        self.path = path

        # for convenience
        self.name = self.info['name']
        self.uid = self.info['uid']
        self.version = self.info['version']
        self.author = self.info['author']
        self.conflicts = self.info['conflicts']
        self.requires = self.info['requires']

    def get_conflicts(self):
        return self.conflicts

    def get_requirements(self):
        return self.requires

    def dump_info(self):
        return self.name, self.author, self.version

    def set_path(self, path):
        self.path = path

    def __eq__(self, other):
        if isinstance(other, type(self)):
            return self.uid == other.uid
        # this way we can use the uid to refer to the object
        if isinstance(other, int):
            return self.uid == other
        return NotImplemented

    def __ne__(self, other):
        if isinstance(other, type(self)):
            return self.uid != other.uid
        if isinstance(other, int):
            return self.uid != other
        return NotImplemented

    def __hash__(self):
        return hash(self.uid)

    def __repr__(self):
        return f'ModInfo<{self.uid}>'

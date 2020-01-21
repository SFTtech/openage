# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Nyan file struct that stores a bunch of objects and
manages imports.
"""

from ....nyan.nyan_structs import NyanObject
from ..data_definition import DataDefinition

FILE_VERSION = "0.1.0"


class NyanFile(DataDefinition):
    """
    Superclass for nyan files.
    """

    def __init__(self, targetdir, filename, nyan_objects=None):
        super().__init__(targetdir, filename)

        self.nyan_objects = set()

        if nyan_objects:
            for nyan_object in nyan_objects:
                self.add_nyan_object(nyan_object)

    def add_nyan_object(self, new_object):
        """
        Adds a nyan object to the file.
        """
        if not isinstance(new_object, NyanObject):
            raise Exception("nyan file cannot contain non-nyan object %s",
                            new_object)

        self.nyan_objects.add(new_object)

        new_fqon = self.targetdir.replace("/", ".")
        new_fqon += self.filename.split(".")[0]
        new_fqon += new_object.get_name()

        new_object.set_fqon(new_fqon)

    def dump(self):
        """
        Returns the string that represents the nyan file.
        """
        output_str = "# NYAN FILE version %s" % (FILE_VERSION)

        # TODO: imports

        for nyan_object in self.nyan_objects:
            output_str += nyan_object.dump()

        return output_str

    def set_filename(self, filename):
        super().set_filename(self, filename)
        self._reset_fqons()

    def set_targetdir(self, targetdir):
        super().set_targetdir(self, targetdir)
        self._reset_fqons()

    def _reset_fqons(self):
        """
        Resets fqons, depending on the current target directory and filename.
        """
        for nyan_object in self.nyan_objects:
            new_fqon = self.targetdir.replace("/", ".")
            new_fqon += self.filename.split(".")[0]
            new_fqon += nyan_object.get_name()

            nyan_object.set_fqon(new_fqon)

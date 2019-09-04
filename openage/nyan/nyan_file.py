# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Nyan file struct that stores a bunch of objects and
manages imports.
"""

from .nyan_structs import NyanObject

FILE_VERSION = "NYAN FILE version 0.1.0"


class NyanFile:
    """
    Superclass for nyan files.
    """

    def __init__(self, targetdir, filename, nyan_objects=None):

        self.targetdir = targetdir
        if not isinstance(filename, str):
            raise ValueError("str expected as filename, not %s" %
                             type(filename))

        self.filename = filename

        self.nyan_objects = set()
        if nyan_objects:
            self.nyan_objects = nyan_objects

    def add_nyan_object(self, new_object):
        """
        Adds a nyan object to the file.
        """
        if isinstance(new_object, NyanObject):
            self.nyan_objects.add(new_object)

        else:
            raise Exception("nyan file cannot contain %s",
                            new_object)

    def save(self):
        """
        Creates a .nyan file from the data.
        """
        with self.targetdir[self.filename].open("wb") as nyan_file:
            nyan_file.write(self.dump())

    def dump(self):
        """
        Returns the string that represents the nyan file.
        """
        output_str = "# %s" % (FILE_VERSION)

        # TODO: imports

        for nyan_object in self.nyan_objects:
            output_str += nyan_object.dump()

        return output_str

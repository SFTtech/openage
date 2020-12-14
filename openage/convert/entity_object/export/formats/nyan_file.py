# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Nyan file struct that stores a bunch of objects and
manages imports.
"""

from .....nyan.nyan_structs import NyanObject
from .....util.ordered_set import OrderedSet
from ..data_definition import DataDefinition


FILE_VERSION = "0.1.0"


class NyanFile(DataDefinition):
    """
    Groups nyan objects into files. Contains methods for creating imports
    and dumping all objects into a human-readable .nyan file.
    """

    def __init__(self, targetdir, filename, modpack_name, nyan_objects=None):
        super().__init__(targetdir, filename)

        self.modpack_name = modpack_name

        self.nyan_objects = OrderedSet()
        if nyan_objects:
            for nyan_object in nyan_objects:
                self.add_nyan_object(nyan_object)

        self.import_tree = None

        self.fqon = (self.modpack_name,
                     *self.targetdir.replace("/", ".")[:-1].split("."),
                     self.filename.split(".")[0])

    def add_nyan_object(self, new_object):
        """
        Adds a nyan object to the file.
        """
        if not isinstance(new_object, NyanObject):
            raise Exception(f"nyan file cannot contain non-nyan object {new_object}")

        self.nyan_objects.add(new_object)

        new_fqon = (*self.fqon, new_object.get_name())
        new_object.set_fqon(new_fqon)

    def dump(self):
        """
        Returns the string that represents the nyan file.
        """
        output_str = f"# NYAN FILE\nversion {FILE_VERSION}\n\n"

        import_aliases = self.import_tree.establish_import_dict(self,
                                                                ignore_names=["type", "types"])

        for alias, fqon in import_aliases.items():
            output_str += "import "

            output_str += ".".join(fqon)

            output_str += f" as {alias}\n"

        output_str += "\n"

        for nyan_object in self.nyan_objects:
            output_str += nyan_object.dump(import_tree=self.import_tree)

        self.import_tree.clear_marks()

        # Removes one empty line at the end of the file
        output_str = output_str[:-1]

        return output_str

    def get_fqon(self):
        """
        Return the fqon of the nyan file
        """
        return self.fqon

    def get_relative_file_path(self):
        """
        Relative path of the nyan file in the modpack.
        """
        return f"{self.modpack_name}/{self.targetdir}{self.filename}"

    def set_import_tree(self, import_tree):
        """
        Sets the import tree of the file.
        """
        self.import_tree = import_tree

    def set_filename(self, filename):
        super().set_filename(filename)
        self._reset_fqons()

    def set_modpack_name(self, modpack_name):
        """
        Set the name of the modpack, the file is contained in.
        """
        self.modpack_name = modpack_name

    def set_targetdir(self, targetdir):
        super().set_targetdir(targetdir)
        self._reset_fqons()

    def _reset_fqons(self):
        """
        Resets fqons, depending on the modpack name,
        target directory and filename.
        """
        for nyan_object in self.nyan_objects:
            new_fqon = (*self.fqon, nyan_object.get_name())

            nyan_object.set_fqon(new_fqon)

        self.fqon = (self.modpack_name,
                     *self.targetdir.replace("/", ".")[:-1].split("."),
                     self.filename.split(".")[0])

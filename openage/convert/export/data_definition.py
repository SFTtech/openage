# Copyright 2014-2020 the openage authors. See copying.md for legal info.

"""
Output format specification for data to write.
"""

from ...util.fslike.path import Path


class DataDefinition:
    """
    Contains a data definition that can then be
    formatted to an arbitrary output file.
    """

    def __init__(self, targetdir, filename):
        """
        Creates a new data definition.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        :param filename: Filename of the resuilting file.
        :type filename: str
        """
        self.set_targetdir(targetdir)
        self.set_filename(filename)

    def dump(self):
        """
        Creates a human-readable string that can be written to a file.
        """
        raise NotImplementedError("%s has not implemented dump() method"
                                  % (type(self)))

    def save(self, exportdir):
        """
        Outputs the contents of the DataDefinition to a file.

        :param exportdir: Relative path to the export directory.
        :type exportdir: ...util.fslike.path.Path
        """
        if not isinstance(exportdir, Path):
            raise ValueError("util.fslike.path.Path expected as filename, not %s" %
                             type(exportdir))

        output_dir = exportdir.joinpath(self.targetdir)
        output_content = self.dump()

        # generate human-readable file
        with output_dir[self.filename].open('wb') as outfile:
            outfile.write(output_content.encode('utf-8'))

    def set_filename(self, filename):
        """
        Sets the filename for the file.

        :param filename: Filename of the resuilting file.
        :type filename: str
        """
        if not isinstance(filename, str):
            raise ValueError("str expected as filename, not %s" %
                             type(filename))

        self.filename = filename

    def set_targetdir(self, targetdir):
        """
        Sets the target directory for the file.

        :param targetdir: Relative path to the export directory.
        :type targetdir: str
        """
        if not isinstance(targetdir, str):
            raise ValueError("str expected as targetdir")

        self.targetdir = targetdir

    def __repr__(self):
        return "DataDefinition<%s>" % (type(self))

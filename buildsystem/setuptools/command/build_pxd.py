# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Command for generating pxd files.
"""

from setuptools import Command

from ...pxdgen import PXDGenerator


class BuildPXDCommand(Command):
    """Command for generating PXD files"""

    description = "build pxd files from the annotations of header files"

    user_options = [
        ('build-dir=', None, "base directory for building pxd files"),
        ('file-list=', None, "the list of header files for which pxd files have to be generated"),
        ('force', 'f', "forcibly generate pxd files (ignore file timestamps)"),
    ]

    boolean_options = ['force']

    def initialize_options(self):
        """Initialize the options required for this command."""
        self.build_dir = None
        self.file_list = None
        self.force = False

    def finalize_options(self):
        """Set final values for the options required by this command."""
        # pylint: disable=W0201
        self.set_undefined_options('build', ('force', 'force'))
        self.file_list = self.file_list.split(';') if self.file_list else []

    def run(self):
        """Execute the command's purpose."""
        for filename in self.file_list:
            generator = PXDGenerator(filename, build_dir=self.build_dir)
            generator.generate(ignore_timestamps=bool(self.force))

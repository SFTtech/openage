# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Custom Distribution class for handling openage's generated setup.cfg file.
"""

import os
from distutils.errors import DistutilsArgError

from setuptools import Distribution

from .command.build import BuildCommand
from .command.build_pxd import BuildPXDCommand
from .command.build_exe import BuildExecutablesCommand
from .command.build_ext import BuildExtensionsCommand


class OpenageDistribution(Distribution):
    """openage's own distribution class"""

    global_options = Distribution.global_options
    global_options.extend([('cfg-file=', None, "Path to setup.cfg file")])

    command_classes = {'build_pxd': BuildPXDCommand,
                       'build_ext': BuildExtensionsCommand,
                       'build_exe': BuildExecutablesCommand,
                       'build': BuildCommand}

    def __init__(self, attrs=None):
        self.exe_modules = None

        self.cfg_file = None
        self.cfg_file_loaded = False

        attrs.setdefault('cmdclass', self.command_classes)
        Distribution.__init__(self, attrs)

    def _load_config_file(self):
        """Loads the config file provided in the cmd line, if not already loaded."""
        if self.cfg_file_loaded:
            return

        if self.cfg_file:
            cfg_file = os.path.abspath(self.cfg_file)
            if not os.path.isfile(cfg_file):
                raise DistutilsArgError('cfg-file "%s" could not be found' % self.cfg_file)
            self.parse_config_files(filenames=[cfg_file])
        self.cfg_file_loaded = True

    def _parse_command_opts(self, parser, args):
        self._load_config_file()
        return Distribution._parse_command_opts(self, parser, args)

    def has_exe_modules(self):
        """Whether we have executables that need to be built."""
        return self.exe_modules and len(self.exe_modules) > 0

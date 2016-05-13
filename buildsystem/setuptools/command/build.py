# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Command for building python library.
"""

from distutils.command.build import build


class BuildCommand(build):
    """Command for building the complete python library"""

    def has_exe_modules(self):
        """Checks if the distribution has executables that need to be built."""
        return self.distribution.has_exe_modules()

    sub_commands = [('build_py', build.has_pure_modules),
                    ('build_clib', build.has_c_libraries),
                    ('build_pxd', None),
                    ('build_ext', build.has_ext_modules),
                    ('build_exe', has_exe_modules),
                    ('build_scripts', build.has_scripts)]

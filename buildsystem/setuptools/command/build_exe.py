# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Command for building embedded executables.
"""

import functools
from distutils import sysconfig
import sys

from .build_ext import BuildExtensionsCommand


def link_executable(compiler, *args, **kwargs):
    """
    We hack the compiler's link_shared_object method to this method to create an
    executable instead of a shared library.
    """
    dummy = [kwargs.pop(key, None) for key in ('export_symbols', 'build_temp')]
    compiler.link_executable(*args, **kwargs)


class BuildExecutablesCommand(BuildExtensionsCommand):
    """Command for building embedded executables"""

    description = "build embedded executables"

    def finalize_options(self):
        """Set final values for the options required by this command."""
        need_options = ('include_dirs', 'rpath', 'cc', 'cxx', 'ar', 'linker', 'cflags')
        self.set_undefined_options('build_ext', *zip(need_options, need_options))

        ext_modules = self.distribution.ext_modules
        self.distribution.ext_modules = self.distribution.exe_modules
        BuildExtensionsCommand.finalize_options(self)
        self.distribution.ext_modules = ext_modules

        python_version = sysconfig.get_config_var('VERSION')
        python_lib_dir = sysconfig.get_config_var('LIBDIR')
        python_lib_name = 'python{version}{abiflags}'.format(version=python_version,
                                                             abiflags=sys.abiflags)
        self.library_dirs.append(python_lib_dir)
        self.libraries.append(python_lib_name)

    def setup_compiler(self):
        """Fix the compiler. Any hacks to the compiler can be made here."""
        BuildExtensionsCommand.setup_compiler(self)
        self.compiler.link_shared_object = functools.partial(link_executable, self.compiler)

    def build_extensions(self):
        """Starting point for executables build"""
        from Cython.Compiler import Options
        Options.embed = "main"

        BuildExtensionsCommand.build_extensions(self)

        Options.embed = None

    def get_ext_fullpath(self, ext_name):
        """Get the full path of the executable"""
        ext_fullpath = BuildExtensionsCommand.get_ext_fullpath(self, ext_name)

        # Remove .so suffix if present, because we are building executables
        ext_suffix = sysconfig.get_config_var('EXT_SUFFIX')
        if ext_fullpath.endswith(ext_suffix):
            ext_fullpath = ext_fullpath[:-len(ext_suffix)]

        return ext_fullpath

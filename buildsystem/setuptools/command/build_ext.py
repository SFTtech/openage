# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
Command for building extensions.
"""

import os
import sys
import functools
from setuptools.command.build_ext import build_ext


def object_filenames(compiler, source_filenames, strip_dir=0, output_dir=''):
    """
    We hack the compiler's object_filenames method to this method for fixing the
    output files, if the source file is already in the output_dir.
    """
    if output_dir:
        sources = []
        for source_filename in source_filenames:
            if source_filename.startswith(output_dir):
                sources.append(os.path.relpath(source_filename, output_dir))
            else:
                sources.append(source_filename)
    else:
        sources = source_filenames

    return compiler.object_filenames_orig(sources, strip_dir=strip_dir, output_dir=output_dir)


class BuildExtensionsCommand(build_ext):
    """Command for building python/cython extensions"""

    user_options = build_ext.user_options
    user_options.extend([
        ('cc=', None, 'CC compiler to be used for building an extension'),
        ('cxx=', None, 'CXX compiler to be used for building an extension'),
        ('linker=', None, 'linker to be used for building an extension'),
        ('ar=', None, 'archiver to be used for building an extension'),
        ('cflags=', None, 'cflags to be used for building an extension'),
    ])
    environ_keys = ['cc', 'cxx', 'ar', 'cflags']

    def initialize_options(self):
        """Initialize the options required for this command."""
        # pylint: disable=C0103
        build_ext.initialize_options(self)
        self.cc = None
        self.cxx = None
        self.ar = None
        self.linker = None
        self.cflags = None

    def finalize_options(self):
        """Set final values for the options required by this command."""
        build_ext.finalize_options(self)
        for environ_key in self.environ_keys:
            value = getattr(self, environ_key, None)
            if value:
                os.environ[environ_key.upper()] = value
        self.library_dirs.extend(self.rpath)

    def setup_compiler(self):
        """Fix the compiler. Any hacks to the compiler can be made here."""
        if self.linker:
            self.compiler.linker_so[0] = self.linker
            self.compiler.linker_exe[0] = self.linker

        self.compiler.object_filenames_orig = self.compiler.object_filenames
        self.compiler.object_filenames = functools.partial(object_filenames, self.compiler)

    def build_extensions(self):
        """Starting point for extensions build"""
        from Cython.Build import cythonize
        new_extensions = cythonize(self.extensions, include_path=self.include_dirs,
                                   build_dir=self.build_temp, annotate=True,
                                   force=bool(self.force), quiet=not bool(self.verbose))
        # This hack is necessary to satisfy setuptools hack
        for i, new_extension in enumerate(new_extensions):
            self.extensions[i].__dict__.update(new_extension.__dict__)

        if sys.platform[:6] == 'darwin':
            for extension in self.extensions:
                extension.extra_link_args.extend(['-Wl,-rpath,' + r_dir for r_dir in self.rpath])

        self.setup_compiler()
        build_ext.build_extensions(self)

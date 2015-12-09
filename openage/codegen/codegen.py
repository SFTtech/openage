# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Utility and driver module for C++ code generation.
"""

import os
from sys import modules
from datetime import datetime
from enum import Enum
from io import UnsupportedOperation
from itertools import chain

from ..util.fslike.wrapper import Wrapper
from ..util.filelike import FIFO
from ..log import err

from .listing import generate_all


class CodegenMode(Enum):
    """
    Modus operandi
    """

    # pylint doesn't understand that this Enum doesn't require member methods.
    # pylint: disable=too-few-public-methods

    # source files are created regularily
    codegen = "codegen"

    # caches are updated, but no source files are created
    dryrun = "dryrun"

    # files are deleted
    clean = "clean"


class WriteCatcher(FIFO):
    """
    Behaves like FIFO, but close() is converted to seteof(),
    and read() fails if eof is not set.
    """
    def close(self):
        self.eof = True

    def read(self, size=-1):
        if not self.eof:
            raise UnsupportedOperation(
                "can not read from WriteCatcher while not closed for writing")
        return super().read(size)


class CodegenDirWrapper(Wrapper):
    """
    Only allows pure-read and pure-write operations;

    Intercepts all writes for later inspection, and logs all reads.

    The constructor takes the to-be-wrapped fslike object.
    """
    def __init__(self, obj):
        super().__init__(obj)

        # stores tuples (parts, intercept_obj), where intercept_obj is a FIFO.
        self.writes = []

        # stores a list of parts.
        self.reads = []

    def open_r(self, parts):
        self.reads.append(parts)
        return super().open_r(parts)

    def open_w(self, parts):
        intercept_obj = WriteCatcher()
        self.writes.append((parts, intercept_obj))
        return intercept_obj

    def get_reads(self):
        """
        Returns an iterable of all path component tuples for files that have
        been read.
        """
        for parts in self.reads:
            yield parts

        self.reads.clear()

    def get_writes(self):
        """
        Returns an iterable of all (path components, data_written) tuples for
        files that have been written.
        """
        for parts, intercept_obj in self.writes:
            yield parts, intercept_obj.read()

        self.writes.clear()

    def __repr__(self):
        return "CodegenDirWrapper({})".format(repr(self.obj))


def codegen(projectdir, mode):
    """
    Calls .listing.generate_all(), and post-processes the generated
    data, checking them and adding a header.
    Writes them to projectdir according to mode. projectdir is a FSLikeObject.

    Returns ({generated}, {depends}), where
    generated is a list of (absolute) filenames of generated files, and
    depends is a list of (absolute) filenames of dependency files.
    """
    # this wrapper intercepts all writes and logs all reads.
    wrapper = CodegenDirWrapper(projectdir)
    generate_all(wrapper.root)

    # set of all generated filenames
    generated = set()

    for parts, data in wrapper.get_writes():
        generated.add(projectdir.fsobj.resolve(parts))
        wpath = projectdir[parts]

        try:
            data = postprocess_write(parts, data)
        except ValueError as exc:
            err("code generation issue with output file " +
                b'/'.join(parts).decode(errors='replace') +
                ":\n" + str(exc.args[0]))
            exit(1)

        if mode == CodegenMode.codegen:
            # skip writing if the file already has that exact content
            try:
                with wpath.open('rb') as outfile:
                    if outfile.read() == data:
                        continue
            except FileNotFoundError:
                pass

            # write new content to file
            wpath.parent.mkdirs()
            with wpath.open('wb') as outfile:
                print(b'/'.join(parts).decode(errors='replace'))
                outfile.write(data)
        elif mode == CodegenMode.dryrun:
            # no-op
            pass
        elif mode == CodegenMode.clean:
            if wpath.is_file():
                print(b'/'.join(parts).decode(errors='replace'))
                wpath.unlink()
        else:
            err("unknown codegen mode: " + str(mode))
            exit(1)

    generated = {os.path.abspath(path).decode() for path in generated}
    depends = {os.path.abspath(path) for path in get_codegen_depends(wrapper)}

    return generated, depends


def depend_module_blacklist():
    """
    Yields all modules whose source files shall explicitly not appear in the
    dependency list, even if they have been imported.
    """
    # openage.config is created only after the first run of cmake,
    # thus, the depends list will change at the second run of codegen,
    # re-triggering cmake.
    try:
        import openage.config
        yield openage.config
    except ImportError:
        pass

    # devmode is imported by config, so the same reason as above applies.
    try:
        import openage.devmode
        yield openage.devmode
    except ImportError:
        pass


def get_codegen_depends(outputwrapper):
    """
    Yields all codegen dependencies.

    outputwrapper is the CodegenDirWrapper that was passed to generate_all;
    it's used to determine the files that have been read.

    In addition, all imported python modules are yielded.
    """
    # add all files that have been read as depends
    for parts in outputwrapper.get_reads():
        yield outputwrapper.obj.fsobj.resolve(parts).decode()

    module_blacklist = set(depend_module_blacklist())

    # add all source files that have been loaded as depends
    for module in modules.values():
        if module in module_blacklist:
            continue

        try:
            filename = module.__file__
        except AttributeError:
            # built-in modules don't have __file__, we don't want those as
            # depends.
            continue

        if module.__package__ == '':
            continue

        if not filename.endswith('.py'):
            print("codegeneration depends on non-.py module " + filename)
            exit(1)

        yield filename


def get_header_lines():
    """
    Yields the lines for the automatically-added file header.
    """

    yield (
        "Copyright 2013-{year} the openage authors. "
        "See copying.md for legal info."
    ).format(year=datetime.now().year)

    yield ""
    yield "Warning: this file was auto-generated; manual changes are futile."
    yield "For details, see buildsystem/codegen.cmake and py/openage/codegen."
    yield ""


def postprocess_write(parts, data):
    """
    Post-processes a single write operation, as intercepted during codegen.
    """
    # test whether filename starts with 'libopenage/'
    if parts[0] != b"libopenage":
        raise ValueError("Not in libopenage source directory")

    # test whether filename matches the pattern *.gen.*
    name, extension = os.path.splitext(parts[-1].decode())
    if not name.endswith('.gen'):
        raise ValueError("Doesn't match required filename format .gen.SUFFIX")

    # check file extension, and use the appropriate comment prefix
    if extension in {'.h', '.cpp'}:
        comment_prefix = '//'
    else:
        raise ValueError("Extension not in {.h, .cpp}")

    datalines = data.decode('ascii').split('\n')
    if 'Copyright' in datalines[0]:
        datalines = datalines[1:]

    headerlines = []
    for line in get_header_lines():
        if line:
            headerlines.append(comment_prefix + " " + line)
        else:
            headerlines.append("")

    return '\n'.join(chain(headerlines, datalines)).encode('ascii')

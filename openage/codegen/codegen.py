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

from ..util.fslike import FSLikeObjectWrapper
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


class CodegenDirWrapper(FSLikeObjectWrapper):
    """
    Only allows pure-read and pure-write operations;

    Intercepts all writes for later inspection, and logs all reads.

    The constructor takes the to-be-wrapped fslike object.
    """
    def __init__(self, wrapped):
        super().__init__(wrapped)

        # stores tuples (pathcomponents, intercept_obj), where
        # intercept_obj is a FIFO.
        self.writes = []

        # stores a list of pathcomponents.
        self.reads = []

    def _open_impl(self, pathcomponents, mode):
        if 'w' in mode:
            # file creation
            intercept_obj = WriteCatcher()
            self.writes.append((pathcomponents, intercept_obj))

            if 'b' in mode:
                return intercept_obj
            else:
                from io import TextIOWrapper
                # use ascii encoding to disallow any non-ascii chars.
                return TextIOWrapper(intercept_obj, encoding='ascii')

        elif 'r' in mode and '+' not in mode:
            self.reads.append(pathcomponents)

            return self.wrapped.open(pathcomponents, mode)

        else:
            raise UnsupportedOperation("illegal open mode: " + str(mode))

    def get_reads(self):
        """
        Returns an iterable of all path component tuples for files that have
        been read.
        """
        for pathcomponents in self.reads:
            yield pathcomponents

        self.reads.clear()

    def get_writes(self):
        """
        Returns an iterable of all (path components, data_written) tuples for
        files that have been written.
        """
        for pathcomponents, intercept_obj in self.writes:
            yield pathcomponents, intercept_obj.read()

        self.writes.clear()


def codegen(projectdir, mode):
    """
    Calls .listing.generate_all(), and post-processes the generated
    data, checking them and adding a header.
    Writes them to projectdir according to mode.

    Returns ({generated}, {depends}), where
    generated is a list of (absolute) filenames of generated files, and
    depends is a list of (absolute) filenames of dependency files.
    """
    wrapper = CodegenDirWrapper(projectdir)
    generate_all(wrapper)

    # set of all generated filenames
    generated = set()

    for pathcomponents, data in wrapper.get_writes():
        generated.add(projectdir.actual_filepath(pathcomponents))

        try:
            data = postprocess_write(pathcomponents, data)
        except ValueError as exc:
            err("code generation issue with output file " +
                '/'.join(pathcomponents) + ":\n" + exc.args[0])
            exit(1)

        if mode == CodegenMode.codegen:
            # skip writing if the file already has that exact content
            try:
                with projectdir.open(pathcomponents, 'rb') as outfile:
                    if outfile.read() == data:
                        continue
            except FileNotFoundError:
                pass

            # write new content to file
            projectdir.mkdirs(pathcomponents[:-1])
            with projectdir.open(pathcomponents, 'wb') as outfile:
                print('/'.join(pathcomponents))
                outfile.write(data)
        elif mode == CodegenMode.dryrun:
            # no-op
            pass
        elif mode == CodegenMode.clean:
            if projectdir.isfile(pathcomponents):
                print(os.path.sep.join(pathcomponents))
                projectdir.remove(pathcomponents)
        else:
            err("unknown codegen mode: " + str(mode))
            exit(1)

    generated = {os.path.abspath(path) for path in generated}
    depends = {os.path.abspath(path) for path in get_codegen_depends(wrapper)}

    return generated, depends


def get_codegen_depends(outputwrapper):
    """
    Yields all codegen dependencies.

    outputwrapper is the CodegenDirWrapper that was passed to generate_all;
    it's used to determine the files that have been read.

    In addition, all imported python modules are yielded.
    """
    # add all files that have been read as depends
    for pathcomponents in outputwrapper.get_reads():
        yield outputwrapper.wrapped.actual_filepath(pathcomponents)

    # add all source files that have been loaded as depends
    for module in modules.values():
        try:
            filename = module.__file__
        except AttributeError:
            # built-in modules don't have __file__, but that's fine.
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


def postprocess_write(pathcomponents, data):
    """
    Post-processes a single write operation, as intercepted during codegen.
    """
    # test whether filename starts with 'cpp/'
    if not pathcomponents[0] == 'libopenage':
        raise ValueError("Not in libopenage source directory")

    # test whether filename matches the pattern *.gen.*
    name, extension = os.path.splitext(pathcomponents[-1])
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

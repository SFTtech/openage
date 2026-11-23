# Copyright 2014-2026 the openage authors. See copying.md for legal info.

"""
Utility and driver module for C++ code generation.
"""

import os
import sys
from datetime import datetime
from enum import Enum
from io import UnsupportedOperation
from itertools import chain
from sys import modules
from typing import Generator, Sequence

from ..log import err
from ..util.filelike.fifo import FIFO
from ..util.fslike.directory import Directory
from ..util.fslike.path import Path, Subpath
from ..util.fslike.wrapper import Wrapper
from .listing import generate_all


class CodegenMode(Enum):
    """
    Modus operandi
    """

    # source files are created regularily
    CODEGEN = "codegen"

    # caches are updated, but no source files are created
    DRYRUN = "dryrun"

    # files are deleted
    CLEAN = "clean"


class WriteCatcher(FIFO):
    """
    Behaves like FIFO, but close() is converted to seteof(),
    and read() fails if eof is not set.
    """

    def close(self) -> None:
        self.eof = True

    def read(self, size: int = -1) -> bytes:
        if not self.eof:
            raise UnsupportedOperation("can not read from WriteCatcher while not closed for writing")
        return super().read(size)


class CodegenPathWrapper(Wrapper):
    """
    Only allows pure-read and pure-write operations;

    Intercepts all writes for later inspection, and logs all reads.

    The constructor takes the to-be-wrapped fslike object.
    """

    def __init__(self, path: Path):
        super().__init__(path)

        # stores tuples (subpath, intercept_obj), where intercept_obj is a FIFO.
        self.writes: list[tuple[Subpath, WriteCatcher]] = []

        # stores a list of subpath.
        self.reads: list[Subpath] = []

    def open_r(self, subpath: Subpath):
        self.reads.append(subpath)
        return super().open_r(subpath)

    def open_w(self, subpath: Subpath):
        intercept_obj = WriteCatcher()
        self.writes.append((subpath, intercept_obj))
        return intercept_obj

    def get_reads(self) -> Generator[Sequence[str], None, None]:
        """
        Returns an iterable of all path component tuples for files that have
        been read.
        """
        yield from self.reads

        self.reads.clear()

    def get_writes(self) -> Generator[tuple[Sequence[str], bytes], None, None]:
        """
        Returns an iterable of all (path components, data_written) tuples for
        files that have been written.
        """
        for subpath, intercept_obj in self.writes:
            yield subpath, intercept_obj.read()

        self.writes.clear()

    def __repr__(self):
        return f"CodegenPathWrapper({self.obj!r})"


def codegen(mode: CodegenMode, input_dir: Directory, output_dir: Directory) -> tuple[set[str], set[str]]:
    """
    Calls .listing.generate_all(), and post-processes the generated
    data, checking them and adding a header.
    Reads the input templates relative to input_dir.
    Writes them to output_dir according to mode. output_dir is a Directory.

    Returns ({generated}, {depends}), where
    generated is a list of (absolute) filenames of generated files, and
    depends is a list of (absolute) filenames of dependency files.
    """
    input_path = input_dir.root
    output_path = output_dir.root

    # this wrapper intercepts all writes and logs all reads.
    wrapper = CodegenPathWrapper(input_path)
    generate_all(wrapper.root)

    # set of all generated filenames
    generated: set[str] = set()

    for subpath, data in wrapper.get_writes():
        # the generated list is written as text
        native_path = (output_path / subpath).resolve_native_path_w()
        if native_path is None:
            raise ValueError(f"could not resolve native path for {output_path}")
        generated.add(native_path)

        # now, actually perform the generation.
        # first, assemble the path for the current file
        wpath = output_path[subpath]

        data = postprocess_write(subpath, data)

        if mode == CodegenMode.CODEGEN:
            # skip writing if the file already has that exact content
            try:
                with wpath.open("rb") as outfile:
                    if outfile.read() == data:
                        continue
            except FileNotFoundError:
                pass

            # write new content to file
            wpath.parent.mkdirs()
            with wpath.open("wb") as outfile:
                print(f"\x1b[36mcodegen: {'/'.join(subpath)}\x1b[0m")
                outfile.write(data)

        elif mode == CodegenMode.DRYRUN:
            # no-op
            pass

        elif mode == CodegenMode.CLEAN:
            if wpath.is_file():
                print("/".join(subpath))
                wpath.unlink()
        else:
            err("unknown codegen mode: %s", mode)
            sys.exit(1)

    depends: set[str] = set()
    for path in get_codegen_depends(wrapper):
        native_path = path.resolve_native_path()
        if native_path is None:
            raise ValueError(f"could not resolve native path for codegen depend {path}")
        depends.add(native_path)

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


def get_codegen_depends(outputwrapper: CodegenPathWrapper) -> Generator[Path, None, None]:
    """
    Yields all codegen dependencies.

    outputwrapper is the CodegenDirWrapper that was passed to generate_all;
    it's used to determine the files that have been read.

    In addition, all imported python modules are yielded.
    """
    # add all files that have been read as depends
    for subpath in outputwrapper.get_reads():
        # this just resolves paths to the output directory
        yield Path((outputwrapper.obj / subpath).resolve_native_path_r())

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

        if filename is None:
            # some modules have __file__ == None, we don't want those either.
            continue

        if module.__package__ == "":
            continue

        if not filename.endswith(".py"):
            # This usually means that some .so file is imported as module.
            # This is not a problem as long as it's not "our" .so file.
            # => just handle non-openage non-.py files normally

            if "openage" in module.__name__:
                print("codegeneration depends on non-.py module " + filename)
                sys.exit(1)

        yield Path(filename)


def get_header_lines() -> Generator[str, None, None]:
    """
    Yields the lines for the automatically-added file header.
    """

    yield (f"Copyright 2013-{datetime.now().year} the openage authors. See copying.md for legal info.")

    yield ""
    yield "Warning: this file was auto-generated; manual changes are futile."
    yield "For details, see buildsystem/codegen.cmake and openage/codegen."
    yield ""


def postprocess_write(subpath: Sequence[str], data: bytes) -> bytes:
    """
    Post-processes a single write operation, as intercepted during codegen.
    """
    # test whether filename starts with 'libopenage/'
    if subpath[0] != "libopenage":
        raise ValueError("Not in libopenage source directory")

    # test whether filename matches the pattern *.gen.*
    name, extension = os.path.splitext(subpath[-1])
    if not name.endswith(".gen"):
        raise ValueError("Doesn't match required filename format .gen.SUFFIX")

    # check file extension, and use the appropriate comment prefix
    if extension in {".h", ".cpp"}:
        comment_prefix = "//"
    else:
        raise ValueError("Extension not in {.h, .cpp}")

    datalines = data.decode("ascii").split("\n")
    if "Copyright" in datalines[0]:
        datalines = datalines[1:]

    headerlines = []
    for line in get_header_lines():
        if line:
            headerlines.append(comment_prefix + " " + line)
        else:
            headerlines.append("")

    return "\n".join(chain(headerlines, datalines)).encode("ascii")

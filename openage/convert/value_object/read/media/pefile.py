# Copyright 2013-2023 the openage authors. See copying.md for legal info.

"""
Provides PEFile, a class for reading MS portable executable files.

Primary doc sources:
http://www.csn.ul.ie/~caolan/pub/winresdump/winresdump/doc/pefile2.html
http://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files
"""

from __future__ import annotations

import typing

from .....util.filelike.stream import StreamFragment
from .....util.struct import NamedStruct

if typing.TYPE_CHECKING:
    from openage.convert.value_object.read.media.peresource import PEResources
    from openage.util.filelike.abstract import FileLikeObject


class PEDOSHeader(NamedStruct):
    """
    The (legacy) DOS-compatible PE header.

    In all modern PE files, only the 'lfanew' pointer is relevant.
    """

    # pylint: disable=too-few-public-methods

    endianness = "<"

    signature: typing.Any = "2s"  # always 'MZ'
    bytes_lastpage: typing.Any = "H"  # bytes on the last page of file
    count_pages: typing.Any = "H"  # pages in file
    crlc: typing.Any = "H"  # relocations
    cparhdr: typing.Any = "H"  # size of header in paragraphs
    minalloc: typing.Any = "H"  # minimum extra paragraphs needed
    maxalloc: typing.Any = "H"  # maximum extra paragraphs needed
    initial_ss: typing.Any = "H"  # initial (relative) SS value
    initial_sp: typing.Any = "H"  # initial sp value
    checksum: typing.Any = "H"  # checksum
    initial_ip: typing.Any = "H"  # initial IP value
    initial_cs: typing.Any = "H"  # initial (relative) CS value
    lfarlc: typing.Any = "H"  # file address of relocation table
    ovno: typing.Any = "H"  # overlay number
    reserved0: typing.Any = "8s"  # reserved block #0
    oemid: typing.Any = "H"  # OEM identifier (for oeminfo)
    oeminfo: typing.Any = "H"  # OEM information; oemid-specific
    reserved1: typing.Any = "20s"  # reserved block #1
    coffheaderpos: typing.Any = "I"  # address of new EXE header


class PECOFFHeader(NamedStruct):
    """
    The new (win32) PE and object file header.
    """

    # pylint: disable=too-few-public-methods

    endianness = "<"

    signature: typing.Any = "4s"  # always 'PE\0\0'
    machine: typing.Any = "H"  # architecture; 332 means x86
    number_of_sections: typing.Any = "H"
    time_stamp: typing.Any = "I"
    symbol_table_ptr: typing.Any = "I"
    symbol_count: typing.Any = "I"
    opt_header_size: typing.Any = "H"
    characteristics: typing.Any = "H"  # 2: exe; 512: non-relocatable; 8192: dll


class PEOptionalHeader(NamedStruct):
    """
    This "optional" header is required for linked files (but not object files).
    """

    # pylint: disable=too-few-public-methods

    endianness = "<"

    signature: typing.Any = "H"  # 267: x86; 523: x86_64
    major_linker_ver: typing.Any = "B"
    minor_linker_ver: typing.Any = "B"
    size_of_code: typing.Any = "I"
    size_of_data: typing.Any = "I"
    size_of_bss: typing.Any = "I"
    entry_point_addr: typing.Any = "I"  # RVA of code entry point
    base_of_code: typing.Any = "I"
    base_of_data: typing.Any = "I"
    image_base: typing.Any = "I"  # preferred memory location
    section_alignment: typing.Any = "I"
    file_alignment: typing.Any = "I"
    major_os_ver: typing.Any = "H"
    minor_os_ver: typing.Any = "H"
    major_img_ver: typing.Any = "H"
    minor_img_ver: typing.Any = "H"
    major_subsys_ver: typing.Any = "H"
    minor_subsys_ver: typing.Any = "H"
    reserved: typing.Any = "I"
    size_of_image: typing.Any = "I"
    size_of_headers: typing.Any = "I"
    checksum: typing.Any = "I"

    # the windows subsystem to run this executable.
    # 1: native, 2: GUI, 3: non-GUI, 5: OS/2, 7: POSIX
    subsystem: typing.Any = "H"

    dll_characteristics: typing.Any = "H"  # some flags we're not interested in.
    stack_reserve_size: typing.Any = "I"
    stack_commit_size: typing.Any = "I"
    heap_reserve_size: typing.Any = "I"
    heap_commit_size: typing.Any = "I"
    loader_flags: typing.Any = "I"  # we're not interested in those either.

    # describes the number of data directory headers that follow this header.
    # always 16.
    data_directory_count: typing.Any = "I"

    # written manually at some later point
    data_directories: list[PEDataDirectory]


class PEDataDirectory(NamedStruct):
    """
    Provides the locations of various metadata structures,
    which are used to set up the execution environment.
    """

    # pylint: disable=too-few-public-methods

    endianness = "<"

    rva: typing.Any = "I"
    size: typing.Any = "I"


class PESection(NamedStruct):
    """
    Describes a section in a PE file (like an ELF section).
    """

    # pylint: disable=too-few-public-methods

    endianness = "<"

    name: typing.Any = "8s"  # first char must be '.'.
    virtual_size: typing.Any = "I"  # size in memory
    virtual_address: typing.Any = "I"  # RVA where the section will be loaded.
    size_on_disk: typing.Any = "I"
    file_offset: typing.Any = "I"
    reserved: typing.Any = "12s"
    flags: typing.Any = "I"  # some flags we don't care about


class PEFile:
    """
    Reads Microsoft PE files.

    The constructor takes a file-like object.
    """

    def __init__(self, fileobj: FileLikeObject):
        # read DOS header
        doshdr = PEDOSHeader.read(fileobj)
        if doshdr.signature != b"MZ":
            raise SyntaxError("not a PE file")

        # read COFF header
        fileobj.seek(doshdr.coffheaderpos)
        coffhdr = PECOFFHeader.read(fileobj)

        if coffhdr.signature != b"PE\0\0":
            raise SyntaxError("not a Win32 PE file")

        if coffhdr.opt_header_size != 224:
            raise SyntaxError("unknown optional header size")

        # read optional header
        opthdr = PEOptionalHeader.read(fileobj)

        if opthdr.signature not in {267, 523}:
            raise SyntaxError("Not an x86{_64} file")

        # read data directories
        opthdr.data_directories = []
        for _ in range(opthdr.data_directory_count):
            opthdr.data_directories.append(PEDataDirectory.read(fileobj))

        # read section headers
        sections: dict[str, PESection] = {}

        for _ in range(coffhdr.number_of_sections):
            section = PESection.read(fileobj)

            section.name = section.name.decode("ascii").rstrip("\0")
            if not section.name.startswith("."):
                raise SyntaxError("Invalid section name: " + section.name)

            sections[section.name] = section

        # store all read header info
        self.fileobj = fileobj

        self.doshdr = doshdr
        self.coffhdr = coffhdr
        self.opthdr = opthdr

        self.sections = sections

    def open_section(self, section_name: str) -> tuple[StreamFragment, int]:
        """
        Returns a tuple of data, va for the given section.

        data is a file-like object (StreamFragment),
        and va is the RVA of the section start.
        """
        if section_name not in self.sections:
            raise SyntaxError("no such section in PE file: " + section_name)

        section = self.sections[section_name]

        return StreamFragment(
            self.fileobj, section.file_offset, section.virtual_size
        ), section.virtual_address

    def resources(self) -> PEResources:
        """
        Returns a PEResources object for self.
        """
        from .peresource import PEResources

        return PEResources(self)

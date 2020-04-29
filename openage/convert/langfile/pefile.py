# Copyright 2013-2017 the openage authors. See copying.md for legal info.

"""
Provides PEFile, a class for reading MS portable executable files.

Primary doc sources:
http://www.csn.ul.ie/~caolan/pub/winresdump/winresdump/doc/pefile2.html
http://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files
"""

from ...util.struct import NamedStruct
from ...util.filelike.stream import StreamFragment


class PEDOSHeader(NamedStruct):
    """
    The (legacy) DOS-compatible PE header.

    In all modern PE files, only the 'lfanew' pointer is relevant.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    signature            = "2s"   # always 'MZ'
    bytes_lastpage       = "H"    # bytes on the last page of file
    count_pages          = "H"    # pages in file
    crlc                 = "H"    # relocations
    cparhdr              = "H"    # size of header in paragraphs
    minalloc             = "H"    # minimum extra paragraphs needed
    maxalloc             = "H"    # maximum extra paragraphs needed
    initial_ss           = "H"    # initial (relative) SS value
    initial_sp           = "H"    # initial sp value
    checksum             = "H"    # checksum
    initial_ip           = "H"    # initial IP value
    initial_cs           = "H"    # initial (relative) CS value
    lfarlc               = "H"    # file address of relocation table
    ovno                 = "H"    # overlay number
    reserved0            = "8s"   # reserved block #0
    oemid                = "H"    # OEM identifier (for oeminfo)
    oeminfo              = "H"    # OEM information; oemid-specific
    reserved1            = "20s"  # reserved block #1
    coffheaderpos        = "I"    # address of new EXE header


class PECOFFHeader(NamedStruct):
    """
    The new (win32) PE and object file header.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    signature            = "4s"   # always 'PE\0\0'
    machine              = "H"    # architecture; 332 means x86
    number_of_sections   = "H"
    time_stamp           = "I"
    symbol_table_ptr     = "I"
    symbol_count         = "I"
    opt_header_size      = "H"
    characteristics      = "H"    # 2: exe; 512: non-relocatable; 8192: dll


class PEOptionalHeader(NamedStruct):
    """
    This "optional" header is required for linked files (but not object files).
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    signature            = "H"    # 267: x86; 523: x86_64
    major_linker_ver     = "B"
    minor_linker_ver     = "B"
    size_of_code         = "I"
    size_of_data         = "I"
    size_of_bss          = "I"
    entry_point_addr     = "I"    # RVA of code entry point
    base_of_code         = "I"
    base_of_data         = "I"
    image_base           = "I"    # preferred memory location
    section_alignment    = "I"
    file_alignment       = "I"
    major_os_ver         = "H"
    minor_os_ver         = "H"
    major_img_ver        = "H"
    minor_img_ver        = "H"
    major_subsys_ver     = "H"
    minor_subsys_ver     = "H"
    reserved             = "I"
    size_of_image        = "I"
    size_of_headers      = "I"
    checksum             = "I"

    # the windows subsystem to run this executable.
    # 1: native, 2: GUI, 3: non-GUI, 5: OS/2, 7: POSIX
    subsystem            = "H"

    dll_characteristics  = "H"    # some flags we're not interested in.
    stack_reserve_size   = "I"
    stack_commit_size    = "I"
    heap_reserve_size    = "I"
    heap_commit_size     = "I"
    loader_flags         = "I"    # we're not interested in those either.

    # describes the number of data directory headers that follow this header.
    # always 16.
    data_directory_count = "I"

    # written manually at some later point
    data_directories     = None


class PEDataDirectory(NamedStruct):
    """
    Provides the locations of various metadata structures,
    which are used to set up the execution environment.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    rva                = "I"
    size               = "I"


class PESection(NamedStruct):
    """
    Describes a section in a PE file (like an ELF section).
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    name               = "8s"    # first char must be '.'.
    virtual_size       = "I"     # size in memory
    virtual_address    = "I"     # RVA where the section will be loaded.
    size_on_disk       = "I"
    file_offset        = "I"
    reserved           = "12s"
    flags              = "I"     # some flags we don't care about


class PEFile:
    """
    Reads Microsoft PE files.

    The constructor takes a file-like object.
    """

    def __init__(self, fileobj):
        # read DOS header
        doshdr = PEDOSHeader.read(fileobj)
        if doshdr.signature != b'MZ':
            raise Exception("not a PE file")

        # read COFF header
        fileobj.seek(doshdr.coffheaderpos)
        coffhdr = PECOFFHeader.read(fileobj)

        if coffhdr.signature != b'PE\0\0':
            raise Exception("not a Win32 PE file")

        if coffhdr.opt_header_size != 224:
            raise Exception("unknown optional header size")

        # read optional header
        opthdr = PEOptionalHeader.read(fileobj)

        if opthdr.signature not in {267, 523}:
            raise Exception("Not an x86{_64} file")

        # read data directories
        opthdr.data_directories = []
        for _ in range(opthdr.data_directory_count):
            opthdr.data_directories.append(PEDataDirectory.read(fileobj))

        # read section headers
        sections = {}

        for _ in range(coffhdr.number_of_sections):
            section = PESection.read(fileobj)

            section.name = section.name.decode('ascii').rstrip('\0')
            if not section.name.startswith('.'):
                raise Exception("Invalid section name: " + section.name)

            sections[section.name] = section

        # store all read header info
        self.fileobj = fileobj

        self.doshdr = doshdr
        self.coffhdr = coffhdr
        self.opthdr = opthdr

        self.sections = sections

    def open_section(self, section_name):
        """
        Returns a tuple of data, va for the given section.

        data is a file-like object (StreamFragment),
        and va is the RVA of the section start.
        """
        if section_name not in self.sections:
            raise Exception("no such section in PE file: " + section_name)

        section = self.sections[section_name]

        return StreamFragment(
            self.fileobj,
            section.file_offset,
            section.virtual_size), section.virtual_address

    def resources(self):
        """
        Returns a PEResources object for self.
        """
        from .peresource import PEResources
        return PEResources(self)

# Copyright 2015-2020 the openage authors. See copying.md for legal info.

"""
Provides CABFile, an extractor for the MSCAB format.

Struct definitions are according to the documentation at
https://msdn.microsoft.com/en-us/library/bb417343.aspx.
"""

from bisect import bisect
from collections import OrderedDict
from calendar import timegm

from ..log import dbg
from ..util.filelike.readonly import PosSavingReadOnlyFileLikeObject
from ..util.filelike.stream import StreamFragment
from ..util.files import read_guaranteed, read_nullterminated_string
from ..util.fslike.filecollection import FileCollection
from ..util.math import INF
from ..util.strings import try_decode
from ..util.struct import NamedStruct, Flags

from .cabchecksum import mscab_csum


class CFHeaderFlags(Flags):
    """ Cabinet file option indicators. Found in the header. """

    # pylint: disable=bad-whitespace,too-few-public-methods

    specstr         = "H"

    # this cabfile is not the first of the set.
    # prev_cab and prev_disk are present after the header.
    prev_cabinet    = 0

    # this cabfile is not the last of the set.
    # next_cab and next_disk are present after the header.
    next_cabinet    = 1

    # this cabinet file has reserved fields.
    # cbCFHeader, cbCFFolder, and cbCFData are present after the header,
    # followed by cbCFHeader bytes of reserved data.
    # if false, cbCFHeader, cbCFFolder, and cbCFData default to 0.
    reserve_present = 2


class CFHeader(NamedStruct):
    """ Global CAB file header; found at the very beginning of the file. """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness      = "<"

    signature       = "4s"  # magic number: MSCF
    reserved1       = "I"   #
    cbCabinet       = "I"   # size of this cabinet file in bytes
    reserved2       = "I"   #
    coffFiles       = "I"   # absolute offset of the first CFFILE entry
    reserved3       = "I"   #
    versionMinor    = "B"   # cab file format version (minor)
    versionMajor    = "B"   # cab file format version (major)
    cFolders        = "H"   # number of CFFOLDER entries in this cabinet
    cFiles          = "H"   # number of CFFILES entries in this cabinet
    flags           = CFHeaderFlags
    setID           = "H"   # must be same in all cabinets of a set
    iCabinet        = "H"   # number of this cabinet file in the set

    # those fields are set manually later.
    reserved_data   = None  # CFHeaderReservedFields

    reserved        = None  # bytes object of size reserved_data.cbCFHeader

    # strings that hold the disk labels/file names where to find the prev/next
    # CAB files.
    prev_cab        = None
    prev_disk       = None
    next_cab        = None
    next_disk       = None


class CFHeaderReservedFields(NamedStruct):
    """
    Optionally found after the header.
    The fields indicate the size of the reserved data blocks in the header,
    folder and data structs.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness      = "<"

    cbCFHeader      = "H"   # size of per-cabinet reserved area
    cbCFFolder      = "B"   # size of per-folder reserved area
    cbCFData        = "B"   # size of per-datablock reserved area


class CFFolder(NamedStruct):
    """
    CAB folder header; A CAB folder is a data stream consisting of
    (compressed) concatenated file contents.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness      = "<"

    coffCabStart    = "I"   # offset of first CFDATA block of this folder.
    cCFData         = "H"   # number of CFDATA blocks.

    typeCompress    = "H"   # compression algorithm for this folder.

    # filled in later manually
    reserved        = None  # bytes object of size reserved_data.cbCFFolder

    comp_name       = None  # human-readable compression name
    plain_stream    = None  # file-like object for decompressed folder.


class CFFileAttributes(Flags):
    """
    File flags; found in the CFFile struct.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    specstr         = "H"

    rdonly          = 0     # read-only
    hidden          = 1     # hidden
    system          = 2     # system file
    arch            = 5     # archive flag: modified since last backup
    exec            = 6     # run file after extraction (lol, we won't.)
    name_is_utf     = 7     # name is UTF-8, not "current locale" (8859-1)


class CFFile(NamedStruct):
    """
    Header for a single file.

    Describes the file's metadata,
    as well as the location of its content (which CAB folder, at what offset).
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness      = "<"

    size            = "I"   # uncompressed filesize
    pos             = "I"   # offset of file in uncompressed folder

    # index of the folder that contains this file.
    # there are several reserved indices with special meanings:
    #
    #  0xFFFD continued_from_prev       (acutal id: 0)
    #  0xFFFE continued_to_next         (acutal id: last)
    #  0xFFFF continued_prev_and_next:  (actual id: 0)
    folderid        = "H"

    date            = "H"   # date stamp ((y–1980) << 9)+(m << 5)+(d) wtf.
    time            = "H"   # time stamp (h << 11)+(m << 5)+(s >> 1) røfl.
    attribs         = CFFileAttributes

    # filled in later manually
    path            = None  # array of path parts

    continued       = None  # file continued from previous CAB file
    continues       = None  # file continues in next CAB file

    folder          = None  # CFFolder object for folderid

    timestamp       = None  # UNIX timestamp


class CFData(NamedStruct):
    """
    CAB folders are concatenations of data blocks; this is the header
    of one such data block.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness      = "<"

    csum            = "I"   # checksum of this block (cbData through payload)
    cbData          = "H"   # number of compressed bytes
    cbUncomp        = "H"   # number of uncompressed bytes

    # filled in later manually
    reserved        = None  # bytes object of size reserved_data.cbCFData
    payload         = None  # compressed folder stream data block

    def verify_checksum(self):
        """
        Checks whether csum contains the correct checksum for the block.
        Raises ValueError otherwise.
        """
        # the first part of the checksum is simply the checksum of cbData and
        # cbUncomp, which is simply the little-endian value of their
        # concatenation.
        checksum = (self.cbUncomp << 16) | self.cbData

        # the next part of the checksum is from reserved.
        if self.reserved:
            checksum ^= mscab_csum(self.reserved)

        # the final part is from the actual data
        checksum ^= mscab_csum(self.payload)

        if checksum != self.csum:
            raise ValueError("checksum error in MSCAB data block")


class CABFile(FileCollection):
    """
    The actual file system-like CAB object.

    Constructor arguments:

    @param cab:
        A file-like object that must implement read() and seek() with
        whence=os.SEEK_SET.

    The constructor reads the entire header, including the folder and file
    descriptions. Most CAB file issues should cause the constructor to fail.
    """
    def __init__(self, cab):
        super().__init__()

        # read header
        cab.seek(0)
        header = CFHeader.read(cab)

        # verify magic number
        if header.signature != b"MSCF":
            raise Exception("invalid CAB file signature: " +
                            repr(header.signature))

        # read reserve header, if present
        if header.flags.reserve_present:
            header.reserved_data = CFHeaderReservedFields.read(cab)
        else:
            header.reserved_data = CFHeaderReservedFields.from_nullbytes()

        # read reserved
        header.reserved = read_guaranteed(cab, header.reserved_data.cbCFHeader)

        # read previous cabinet info
        if header.flags.prev_cabinet:
            header.prev_cab = try_decode(read_nullterminated_string(cab))
            header.prev_disk = try_decode(read_nullterminated_string(cab))

        # read next cabinet info
        if header.flags.next_cabinet:
            header.next_cab = try_decode(read_nullterminated_string(cab))
            header.next_disk = try_decode(read_nullterminated_string(cab))

        dbg(header)
        self.header = header

        self.folders = tuple(self.read_folder_headers(cab))

        # {filename: fileobj}, {subdirname: subdir}
        self.rootdir = OrderedDict(), OrderedDict()

        for fileobj in self.read_file_headers(cab):
            if self.is_file(fileobj.path) or self.is_dir(fileobj.path):
                raise ValueError(
                    "CABFile has multiple entries with the same path: " +
                    b'/'.join(fileobj.path).decode())

            def open_r(fileobj=fileobj):
                """ Returns a opened ('rb') file-like object for fileobj. """
                return StreamFragment(
                    fileobj.folder.plain_stream,
                    fileobj.pos,
                    fileobj.size
                )

            self.add_fileentry(fileobj.path, (
                open_r,
                None,
                lambda fileobj=fileobj: fileobj.size,
                lambda fileobj=fileobj: fileobj.timestamp
            ))

    def __repr__(self):
        return "CABFile"

    def read_folder_headers(self, cab):
        """
        Called during the constructor run.

        Reads the folder headers and initializes the folder's plain stream
        file-like objects.

        Yields all folders.
        """
        # read folder headers
        for _ in range(self.header.cFolders):
            folder = CFFolder.read(cab)

            # read reserved
            folder.reserved = read_guaranteed(
                cab,
                self.header.reserved_data.cbCFFolder)

            # create compressed data stream
            compressed_data_stream = CABFolderStream(
                cab,
                folder.coffCabStart,
                folder.cCFData,
                self.header.reserved_data.cbCFData)

            # determine compression type and create plain data stream
            compression_type = folder.typeCompress & 0x000f

            if compression_type == 0:
                folder.comp_name = "Plain"
                folder.plain_stream = compressed_data_stream

            elif compression_type == 1:
                raise Exception("MSZIP compression is unsupported")

            elif compression_type == 2:
                raise Exception("Quantum compression is unsupported")

            elif compression_type == 3:
                window_bits = (folder.typeCompress >> 8) & 0x1f
                folder.comp_name = "LZX (window_bits = %d)" % window_bits

                from .lzxdstream import LZXDStream
                from ..util.filelike.stream import StreamSeekBuffer

                unseekable_plain_stream = LZXDStream(
                    compressed_data_stream,
                    window_bits=window_bits,
                    reset_interval=0)

                folder.plain_stream = StreamSeekBuffer(unseekable_plain_stream)

            else:
                raise Exception("Unknown compression type %d"
                                % compression_type)

            dbg(folder)
            yield folder

    def read_file_headers(self, cab):
        """
        Called during the constructor run.

        Reads the headers for all files and yields CFFile objects.
        """
        # seek to the correct position
        if cab.tell() != self.header.coffFiles:
            cab.seek(self.header.coffFiles)
            dbg("cabfile has nonstandard format: seek to header.coffFiles was required")

        for _ in range(self.header.cFiles):
            fileobj = CFFile.read(cab)

            # read filename
            rpath = read_nullterminated_string(cab)

            # decode filename according to flags
            if fileobj.attribs.name_is_utf:
                path = rpath.decode('utf-8')
            else:
                path = rpath.decode('iso-8859-1')

            fileobj.path = path.replace('\\', '/').lower().encode().split(b'/')

            # interpret the special values of folderid
            if fileobj.folderid == 0xFFFD:
                fileobj.folderid = 0
                fileobj.continued = True
            elif fileobj.folderid == 0xFFFE:
                fileobj.folderid = len(self.folders) - 1
                fileobj.continues = True
            elif fileobj.folderid == 0xFFFF:
                fileobj.folderid = 0
                fileobj.continued = True
                fileobj.continues = True

            fileobj.folder = self.folders[fileobj.folderid]

            # decode file timestamp
            # beware: reading this may give you internal bleedings.
            year = (fileobj.date >> 9) + 1980
            month = (fileobj.date >> 5) & 0x000f
            day = (fileobj.date >> 0) & 0x001f

            # it's sort of sad that there's no bit for AM/PM.
            hour = (fileobj.time >> 11)
            minute = (fileobj.time >> 5) & 0x003f
            sec = (fileobj.time << 1) & 0x003f

            # CAB files have no timezone info; assume UTC.
            fileobj.timestamp = timegm((year, month, day, hour, minute, sec))

            yield fileobj


class CABFolderStream(PosSavingReadOnlyFileLikeObject):
    """
    Read-only, seekable, file-like stream object that represents
    a compressed MSCAB folder (MSCAB folders are just compressed streams
    of concatenated file contents, and are not to be confused with file system
    folders).

    Constructor arguments:

    @param fileobj:
        Seekable file-like object that represents the CAB file.

        CABFolderStream explicitly positions the file cursor before every read;
        this is to make sure that multiple CABFolderStreams can work on the
        same file, in parallel.

        The file object must implement read() and seek() with SEEK_SET.

    @param offset:
        Offset of the first of the folder's data blocks in the CAB file.

    @param blockcount:
        Number of data blocks in the folder.
    """

    def __init__(self, fileobj, offset, blockcount, blockreserved):
        super().__init__()

        self.fileobj = fileobj
        self.blockcount = blockcount
        self.blockreserved = blockreserved

        # positions of the blocks in fileobj. block 0 starts at offset.
        self.blockoffsets = [offset]

        # positions in the stream of the start of each block.
        self.streamindex = [0]

    def next_block_size(self, payloadsize):
        """
        adds metadata for the next block
        """
        self.blockoffsets.append(self.blockoffsets[-1] +
                                 CFData.size() + self.blockreserved +
                                 payloadsize)

        self.streamindex.append(self.streamindex[-1] + payloadsize)

    def read_block_data(self, block_id):
        """
        reads the data of block block_id.

        if necessary, the metadata info in self.blockvalues and
        self.blockoffsets is updated.

        returns the block data.
        """
        if block_id >= self.blockcount:
            raise EOFError()

        while block_id >= len(self.blockoffsets):
            # We do not yet know where the block starts. Seek forwards.

            # read info for the rightmost known block to get its size.
            offset = self.blockoffsets[-1]
            self.fileobj.seek(self.blockoffsets[-1])
            datablock = CFData.read(self.fileobj)

            # add starting position of next block to metadata.
            self.next_block_size(datablock.cbData)

        # we now know the starting position of the block.
        offset = self.blockoffsets[block_id]
        self.fileobj.seek(offset)
        datablock = CFData.read(self.fileobj)
        datablock.reserved = read_guaranteed(self.fileobj, self.blockreserved)
        datablock.payload = read_guaranteed(self.fileobj, datablock.cbData)

        # verify the datablock's checksum.
        datablock.verify_checksum()

        # add starting data of next block to metadata, if required.
        if block_id + 1 == len(self.blockoffsets):
            self.next_block_size(datablock.cbData)

        # finally, return the data.
        return datablock.payload

    def read_blocks(self, size=-1):
        """
        Similar to read, bit instead of a single bytes object,
        returns an iterator of multiple bytes objects, one for each block.

        Used internally be read(), but you may use it directly.
        """
        if size < 0:
            size = INF

        # use self.streamindex to determine the block id for pos.
        blockid = bisect(self.streamindex, self.pos) - 1

        # bytes to discard (because we're still reading data that lie
        # before self.pos).
        discard = self.pos - self.streamindex[blockid]

        while size > 0:
            try:
                block_data = self.read_block_data(blockid)
            except EOFError:
                return

            blockid += 1

            if discard != 0:
                if discard >= len(block_data):
                    # we're actually still seeking through the file,
                    # reading it block by block, approaching the stream cursor
                    # position.

                    discard -= len(block_data)
                    continue

                # discard the first few bytes of the block's data.
                block_data = block_data[discard:]
                discard = 0

            if len(block_data) > size:
                # less than the entire block was requested.
                block_data = block_data[:size]

            size -= len(block_data)
            self.pos += len(block_data)

            yield block_data

    def read(self, size=-1):
        return b"".join(self.read_blocks(size))

    def get_size(self):
        del self  # unused
        return -1

    def close(self):
        self.closed = True
        del self.fileobj
        del self.blockoffsets
        del self.streamindex

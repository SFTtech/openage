#!/usr/bin/env python3

import os
from struct import Struct
from bisect import bisect
import lzxd

class Error(Exception):
    pass

class Namespace:
    def __init__(self, *mappings, **kwargs):
        self._update(*mappings, **kwargs)

    def _update(self, *mappings, **kwargs):
        for mapping in mappings:
            if type(mapping) != dict:
                mapping = vars(mapping)
            self._update(**mapping)

        for k, v in kwargs.items():
            self[k] = v

    def __getitem__(self, k):
        if k[0] == '_':
            raise Exception("key mustn't start with _")
        return getattr(self, k)

    def __setitem__(self, k, v):
        if k[0] == '_':
            raise Exception("key mustn't start with _")
        setattr(self, k, v)

    def __str__(self):
        result = []
        items = sorted(vars(self).items())
        klen = max((len(k) for k, v in items))
        for k, v in items:
            result.append(k.ljust(klen) + " = " + str(v))

        return '\n'.join(result)

    def __repr__(self):
        return repr(vars(self))

class ByteQueue:
    def __init__(self):
        import collections
        self.bufs = collections.deque()
        self.size = 0
        self.pos = 0

    def __len__(self):
        return self.size

    def append(self, buf):
        self.bufs.append(buf)
        self.size += len(buf)

    def popleft(self, size):
        if size > self.size:
            raise Exception("queue does not contain enough bytes")

        self.size -= size
        self.pos += size

        resultbufs = []
        required = size

        while required > 0:
            buf = self.bufs.popleft()
            resultbufs.append(buf)
            required -= len(buf)

        if required < 0:
            # we requested too much; split the last buffer
            buf = resultbufs.pop()

            popped = buf[:required]
            kept = buf[required:]

            resultbufs.append(popped)
            self.bufs.appendleft(kept)

        return b"".join(resultbufs)

class IterCache:
    def __init__(self, iterator):
        self.iterator = iterator
        self.next()

    def next(self):
        self.current = next(self.iterator)

def read_bytes(f, count):
    data = f.read(count)
    if len(data) != count:
        raise Error("Unexpected EOF")
    return data

def read_at(f, pos, count):
    oldpos = f.tell()
    f.seek(pos)
    data = read_bytes(f, count)
    f.seek(oldpos)
    return data

def read_string(f, maxlen = 255):
    result = b""
    while True:
        c = read_bytes(f, 1)
        if c == b"\0":
            return result
        else:
            result += c

        if len(result) > maxlen:
            raise Error("expected \\0; string too long")

def try_decode(s):
    try:
        return s.decode('utf-8')
    except:
        return s.decode('iso-8859-1')

class NamedMemberStruct:
    def __init__(self, endianness, *members):
        specstr = endianness
        self.members = []
        self.membertypes = []
        for membertype, membername in members:
            specstr += membertype
            if membertype == "x":
                continue
            self.members.append(membername)
            self.membertypes.append(membertype)

        self.struct = Struct(specstr)

    def read(self, f):
        vals = self.struct.unpack(read_bytes(f, self.struct.size))
        return Namespace({k: v for v, k in zip(vals, self.members)})

    def default(self):
        result = Namespace()
        for membertype, name in zip(self.membertypes, self.members):
            if membertype in "?":
                val = False
            elif membertype in "cbBhHiIlLfdnNPqQ":
                val = 0
            elif membertype[-1] in "sp":
                val = b""
            else:
                raise Error("Unknown default for type " + membertype)
            result[name] = val
        return result

class FlagDecoder:
    def __init__(self, *flags):
        self.flags = []
        for bitpos, name in flags:
            self.flags.append(((1 << bitpos), name))

    def decode(self, val):
        result = Namespace()
        for flagweight, flagname in self.flags:
            if val & flagweight:
                hasflag = True
                val ^= flagweight
            else:
                hasflag = False
            result[flagname] = hasflag
        if val:
            raise Error("Undefined flag values: " + str(flag))
        return result

    def default(self):
        return Namespace({name: False for _, name in self.flags})

cfheaderflags_decoder = FlagDecoder(
    (0, "prev_cabinet"),   # this cabfile is not the first of the set.
                           # szCabinetPrev and szDiskPrev are present
                           # after the header.
    (1, "next_cabinet"),   # this cabfile is not the last of the set.
                           # szCabinetNext and szDiskNext are present
                           # after the header.
    (2, "reserve_present") # this cabinet file has reserved fields.
                           # cbCFHeader, cbCFFolder, and cbCFData
                           # are present after the header, followed by
                           # cbCFHeader bytes of data.
)

cfheader_struct = NamedMemberStruct("<",
    ("4s", "signature"),    # magic number: MSCF
    ("I",  "reserved1"),    #
    ("I",  "cbCabinet"),    # size of this cabinet file in bytes
    ("I",  "reserved2"),    #
    ("I",  "coffFiles"),    # absolute offset of the first CFFILE entry
    ("I",  "reserved3"),    #
    ("B",  "versionMinor"), # cab file format version (minor)
    ("B",  "versionMajor"), # cab file format version (major)
    ("H",  "cFolders"),     # number of CFFOLDER entries in this cabinet
    ("H",  "cFiles"),       # number of CFFILES entries in this cabinet
    ("H",  "flags"),        # see cfheaderflags_decoder
    ("H",  "setID"),        # must be same in all cabinets of a set
    ("H",  "iCabinet"),     # number of this cabinet file in the set
)

cfheader_reservedfields_struct = NamedMemberStruct("<",
    ("H",  "cbCFHeader"),   # size of per-cabinet reserved area
    ("B",  "cbCFFolder"),   # size of per-folder reserved area
    ("B",  "cbCFData"),     # size of per-datablock reserved area
)

cffolder_struct = NamedMemberStruct("<",
    ("I",  "coffCabStart"), # absolute file offset of first CFDATA block
    ("H",  "cCFData"),      # number of CFDATA blocks
    ("H",  "typeCompress"), # compression type. only the 4 last significant
                            # bits seem to be relevant.
)

cffileattribsflag_decoder = FlagDecoder(
    (0, "rdonly"),          # readonly
    (1, "hidden"),          # hidden
    (2, "system"),          # system file
    (5, "arch"),            # archive flag: modified since last backup
    (6, "exec"),            # run after extraction
    (7, "name_is_utf"),     # name is UTF-8, not "current locale" (8859-1)
)

cffile_struct = NamedMemberStruct("<",
    ("I",  "cbFile"),          # uncompressed filesize
    ("I",  "uoffFolderStart"), # uncompressed offset of file in folder
    ("H",  "iFolder"),         # index of the folder that contains this file.
                               # there are several special indices:
                               # 0xFFFD continued_from_prev: 0
                               # 0xFFFE continued_to_next: -1
                               # 0xFFFF continued_prev_and_next: 0
    ("H",  "date"),            # date stamp ((y–1980) << 9)+(m << 5)+(d) wtf.
    ("H",  "time"),            # time stamp (h << 11)+(m << 5)+(s >> 1) røfl.
    ("H",  "attribs"),         # see cffileattribsflag_decoder
)

cfdata_struct = NamedMemberStruct("<",
    ("I",  "csum"),            # checksum of cbData through ab
    ("H",  "cbData"),          # number of compressed bytes
    ("H",  "cbUncomp"),        # number of uncompressed bytes
    # abReserve[header.cbCFData],
    # ab[cbData]
)

class CABFile:
    def __init__(self, filename, readfiledata=False):
        f = open(filename, 'rb')

        # read header
        header = cfheader_struct.read(f)
        if header.signature != b"MSCF":
            raise Error("invalid CAB file signature")

        # decode flags
        header._update(cfheaderflags_decoder.decode(header.flags))

        # read reserve header (optional)
        if header.reserve_present:
            header._update(cfheader_reservedfields_struct.read(f))
        else:
            header._update(cfheader_reservedfields_struct.default())

        # read abReserve
        header.abReserve = read_bytes(f, header.cbCFHeader)

        # read previous cabinet info
        if header.prev_cabinet:
            header.szCabinetPrev = try_decode(read_string(f))
            header.szDiskPrev = try_decode(read_string(f))
        else:
            header.szCabinetPrev = None
            header.szDiskPrev = None

        # read next cabinet info
        if header.next_cabinet:
            header.szCabinetNext = try_decode(read_string(f))
            header.szDiskNext = try_decode(read_string(f))
        else:
            header.szCabinetNext = None
            header.szDiskNext = None

        folders = []

        # read folder headers
        for i in range(header.cFolders):
            folder = cffolder_struct.read(f)

            # read abReserve
            folder.abReserve = read_bytes(f, header.cbCFFolder)
            folder.comptype_masked = folder.typeCompress & 0x000f
            folder.comp_window_size = (folder.typeCompress >> 8) & 0x1f
            if folder.comptype_masked == 0:
                folder.comptype = "plain"
                raise Error("plain compression is unsupported (uhm, yes.)")
            elif folder.comptype_masked == 1:
                folder.comptype = "MSZIP"
                raise Error("MSZIP compression is unsupported")
            elif folder.comptype_masked == 2:
                folder.comptype = "QUANTUM"
                raise Error("Quantum compression is unsupported")
            elif folder.comptype_masked == 3:
                folder.comptype = "LZX"
            else:
                raise Error("Unknown compression type: " +
                            str(folder.comptype_masked))

            folder.index = i
            folders.append(folder)

        # read file headers
        if f.tell() != header.coffFiles:
            f.seek(header.coffFiles)
            print("cabfile has nonstandard format: seek to header.coffFiles " +
                  "was required")

        files = {}
        files_lowercase = {}

        for _ in range(header.cFiles):
            file_ = cffile_struct.read(f)


            # decode flags
            file_._update(cffileattribsflag_decoder.decode(file_.attribs))

            # read filename
            file_.name = read_string(f)

            # decode filename according to flags
            if file_.name_is_utf:
                file_.name = file_.name.decode('utf-8')
            else:
                file_.name = file_.name.decode('iso-8859-1')

            file_.continued_from_prev = False
            file_.continued_to_next = False
            if file_.iFolder == 0xFFFD:
                file_.iFolder = 0
                file_.continued_from_prev = True
            elif file_.iFolder == 0xFFFE:
                file_.iFolder = len(folders) - 1
                file_.continued_to_next = True
            elif file_.iFolder == 0xFFFF:
                file_.iFolder = 0
                file_.continued_from_prev = True
                file_.continued_to_next = True

            year =   (file_.date >> 9) + 1980
            month =  (file_.date >> 5) & 0x000f
            day =    (file_.date >> 0) & 0x001f
            hour =   (file_.time >> 11)
            minute = (file_.time >> 5) & 0x003f
            sec =    (file_.time << 1) & 0x003f

            import datetime
            dt = datetime.datetime(year, month, day, hour, minute, sec)
            import calendar
            file_.timestamp = calendar.timegm(dt.utctimetuple())
            # no timezone info is available; assume UTC.

            # cosmetic changes
            file_.folderid = file_.iFolder
            file_.name = file_.name.replace('\\', '/')
            file_.size = file_.cbFile
            file_.pos = file_.uoffFolderStart

            # insert file into dicts
            if file_.name in files:
                raise Exception("multiple files with name %s" % file_.name)
            files[file_.name] = file_
            lowername = file_.name.lower()
            if lowername in files_lowercase:
                raise Exception("multiple files with lower-case name %s"
                                % lowername)
            files_lowercase[lowername] = file_

        # read data block metainfo
        for folder in folders:
            folder.datablocks = []
            if f.tell() != folder.coffCabStart:
                print("cabfile has strange format: seek to first data block" +
                      "of folder " + str(folder.index) + " is required.")
                f.seek(folder.coffCabStart)

            folder.uncompressed_size = 0
            for _ in range(folder.cCFData):
                datablock = cfdata_struct.read(f)
                datablock.abReserve = read_bytes(f, header.cbCFData)
                datablock.ab_start = f.tell()
                datablock.ab_size = datablock.cbData
                f.seek(datablock.ab_size, 1)
                # checksum is unimplemented for now
                folder.datablocks.append(datablock)
                folder.uncompressed_size += datablock.cbUncomp

            folder.pseudofile = FolderPseudoFile(folder, f)

        self.f = f
        self.header = header
        self.folders = folders
        self.files = files
        self.files_lowercase = files_lowercase

        self.filedata_read = False
        if readfiledata:
            self.readfiledata()

    def readfiledata(self):
        def listfiles():
            """
            will yield metadata for all files that are contained in this cab,
            in the exact order in which their data is contained in the
            uncompressed folder(s).
            """
            for f in sorted(self.files.values(),
                            key=lambda f: (f.folderid, f.pos)):
                yield f

            # a "None" file
            yield Namespace(folderid=None)

        # files.current is the currently returned file
        # files.next() seeks the next file
        files = IterCache(listfiles())
        iter(iter([1]))

        for folderid, folder in enumerate(self.folders):
            buf = ByteQueue()

            def decomp_write(data):
                buf.append(data)

                # check whether currentbuf contains complete files
                while True:
                    if folderid != files.current.folderid:
                        # there are no more files in this folder
                        break

                    if files.current.pos > buf.pos:
                        # the current file does _not_ begin at the start of
                        # the buffer; there are some garbage bytes!
                        discard = min(len(buf), files.current.pos - buf.pos)
                        buf.pop(discard)
                        print("warning: discarding %d bytes in folder %d!"
                              % (discard, folderid))

                    if files.current.pos < buf.pos:
                        # the beginning of the file is missing
                        raise Exception("File start position invalid: %s"
                                        % (files.current.name))

                    if len(buf) < files.current.size:
                        # the file data is not yet fully loaded into buf.
                        break

                    files.current.data = buf.popleft(files.current.size)
                    files.next()

                # the callback API wants us to return len(data)
                return len(data)

            if folder.comptype == 'LZX':
                lzxd.decompress(folder.comp_window_size, folder.uncompressed_size, folder.pseudofile.read, decomp_write)
            else:
                raise Exception("Unsupported folder compression: " + folder.comptype)

            if buf:
                print("warning: the last %d bytes of folder %d are being discarded!" % (len(currentbuf), folderid))

            if files.current.folderid == folderid:
                raise Exception("Unexpected end of folder %d while reading data for %s" % (folderid, meta[fileno][2].name))

        if files.current.folderid != None:
            raise Exception("Missing folder: %d" % (files.current.folderid))

        self.filedata_read = True

    def open(self, name, mode='rb', ignorecase=True):
        if mode != 'rb':
            raise Exception("mode most be 'rb'")

        if ignorecase:
            f = self.files_lowercase[name.lower()]
        else:
            f = self.files[name]

        if not self.filedata_read:
            self.readfiledata()

        return BlobPseudoFile(f.data)


class BlobPseudoFile:
    def __init__(self, blob):
        self.blob = blob
        self.pos = 0

    def tell(self):
        return self.pos

    def seek(self, offset, fromwhat = 0):
        if fromwhat == 0:
            self.pos = offset + 0
        elif fromwhat == 1:
            self.pos = offset + self.pos
        elif fromwhat == 2:
            self.pos = offset + len(self.blob)

        return self.pos

    def read(self, size=-1):
        if size < 0:
            size = float("+inf")
        size = max(0, min(size, len(self.blob) - self.pos))

        return self.blob[self.pos:self.pos + size]
        self.pos += size


class FolderPseudoFile:
    def __init__(self, folder, f):
        self.f = f
        self.pos = 0
        self.size = 0
        self.datablockanchors = []
        self.datablocks = []
        for db in folder.datablocks:
            self.datablockanchors.append(self.size)
            self.datablocks.append((db.ab_start, db.ab_size)) # physical start, size
            self.size += db.ab_size

    def close(self):
        del self.f

    def current_datablock(self):
        return bisect(self.datablockanchors, self.pos) - 1

    def datablock_remaining(self, idx):
        return max(0, self.datablock_size(idx) - self.pos_in_datablock(idx))

    def datablock_size(self, idx):
        return self.datablocks[idx][1]

    def datablock_physicalstart(self, idx):
        return self.datablocks[idx][0]

    def pos_in_datablock(self, idx):
        return self.pos - self.datablockanchors[idx]

    def tell(self):
        return self.pos

    def seek(self, offset, fromwhat = 0):
        if fromwhat == 0:
            self.pos = offset + 0
        elif fromwhat == 1:
            self.pos = offset + self.pos
        elif fromwhat == 2:
            self.pos = offset + self.size

        return self.pos

    def read(self, size=-1):
        if size < 0:
            size = float("+inf")

        # find the datablock we're currently in
        idx = self.current_datablock()
        result = []
        while True:
            # remaining data in this block
            rem = self.datablock_remaining(idx)
            bytecount = min(rem, size)

            # this is the last datablock we need
            result.append(read_at(self.f, self.datablock_physicalstart(idx) + self.pos_in_datablock(idx), bytecount))
            self.pos += bytecount
            size -= bytecount

            if size == 0:
                # done reading
                return b"".join(result)

            if self.pos >= self.size:
                # EOF
                return b"".join(result)

            # move on to next datablock
            idx += 1

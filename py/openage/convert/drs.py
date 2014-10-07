from . import util
from .util import dbg
from binascii import hexlify
from struct import Struct, unpack_from

#version of the drs file, hardcoded for now
file_version = 57

if file_version == 57:
    copyright_size = 40
elif file_version == 59:
    copyright_size = 60

#little endian byte-order
endianness = "< "

class DRS:
    #struct drs_header {
    # char copyright[copyright-size];
    # char version[4];
    # char ftype[12];
    # int table_count;
    # int file_offset; //offset of first file
    #};
    #
    drs_header = Struct(endianness + str(copyright_size) + "s 4s 12s i i")

    #struct table_info {
    # char file_type;
    # char file_extension[3]; //reversed extension
    # int file_info_offset;   //table offset
    # int file_count;          //number of files in table
    #};
    drs_table_info = Struct(endianness + "c 3s i i")

    #struct file_info {
    # int file_id;
    # int file_data_offset;
    # int file_size;
    #};
    drs_file_info = Struct(endianness + "i i i")

    def __init__(self, fname):
        self.files = {}   #(extension, id): (data offset, size)

        self.fname = fname
        fname = util.file_get_path(fname, write = False)
        f = util.file_open(fname, binary = True, write = False)

        #read header
        buf = f.read(DRS.drs_header.size)
        self.header = DRS.drs_header.unpack(buf)

        dbg("DRS header [%s]" % (fname), 1, push = "drs")
        dbg("copyright:          %s" % util.zstr(self.header[0]))
        dbg("version:            %s" % util.zstr(self.header[1]))
        dbg("ftype:              %s" % util.zstr(self.header[2]))
        dbg("table count:        %d" % (self.header[3]))
        dbg("file offset:        %d" % (self.header[4]))
        dbg("")

        #read table info
        table_count = self.header[3]

        table_header_buf = f.read(table_count * DRS.drs_table_info.size)
        for i in range(table_count):
            table_header = DRS.drs_table_info.unpack_from(table_header_buf, i * DRS.drs_table_info.size)
            file_type, file_extension, file_info_offset, file_count = table_header

            #flip the extension... it's stored that way...
            file_extension = file_extension.decode('latin-1').lower()[::-1]

            dbg("Table header [%d]" % i, 2, push = "table")
            dbg("file type:        0x%s"  % hexlify(file_type).decode('utf-8'))
            dbg("file extension:   %s"    % (file_extension))
            dbg("file_info_offset: %#08x" % (file_info_offset))
            dbg("file_count:       %d"    % file_count)
            dbg("")

            f.seek(file_info_offset)
            file_info_buf = f.read(file_count * DRS.drs_file_info.size)

            for j in range(file_count):
                file_header = DRS.drs_file_info.unpack_from(file_info_buf, j * DRS.drs_file_info.size)
                file_id, file_data_offset, file_size = file_header

                dbg("File info header [%d]" % j, 3, push = "fileinfo")
                dbg("file id:        %d" % (file_id))
                dbg("data offset:    %d" % (file_data_offset))
                dbg("file size:      %d" % (file_size))
                dbg("")

                self.files[(file_extension, file_id)] = file_data_offset, file_size
                dbg(pop = "fileinfo")

            dbg(pop = "table")

        self.f = f

        dbg(pop = "drs")

    def get_file_data(self, file_extension, file_id):
        file_data_offset, file_size = self.files[(file_extension, file_id)]

        self.f.seek(file_data_offset)
        return self.f.read(file_size)

    def __repr__(self):
        return "DRS file (%d tables, %d files)" % (self.header[3], len(self.files))

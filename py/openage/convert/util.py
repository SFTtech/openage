# Copyright 2013-2014 the openage authors. See copying.md for legal info.

import os
import os.path

from openage.log import dbg


readpath = "/dev/null"
writepath = "/dev/null"


def mkdirs(path):
    os.makedirs(path, exist_ok=True)


def set_write_dir(dirname):
    global writepath
    writepath = dirname


def set_read_dir(dirname):
    global readpath
    readpath = dirname


def file_get_path(fname, write=False, mkdir=False):
    global writepath, readpath

    if write:
        basedir = writepath
    else:
        basedir = readpath

    path = os.path.join(basedir, fname)

    if mkdir:
        dirname = os.path.dirname(path)
        mkdirs(dirname)

    return path


def file_open(path, binary=True, write=False):
    if write:
        flags = 'w'
    else:
        flags = 'r'
    if binary:
        flags += 'b'

    return open(path, flags)


def file_write(fname, data):
    """
    writes data to a file with given name.
    """

    # ensure that the directory exists
    mkdirs(os.path.dirname(fname))

    if type(data) == bytes:
        handle = file_open(fname, binary=True, write=True)
        handle.write(data)
        handle.close()
    elif type(data) == str:
        handle = file_open(fname, binary=False, write=True)
        handle.write(data)
        handle.close()
    else:
        raise Exception("Unknown data type for writing: " + str(type(data)))


def file_write_multi(file_dict, file_prefix=""):
    """
    save the given file dictionary to files

    key: filename
    value: file content
    """

    written_files = list()

    for file_name, file_data in file_dict.items():
        file_name = file_prefix + file_name
        dbg("saving %s.." % file_name, 1)
        file_name = file_get_path(file_name, write=True, mkdir=True)
        file_write(file_name, file_data)

        written_files.append(file_name)

    return written_files


def file_write_image(filename, image):
    """
    writes a PIL image to a filename
    """

    filename = file_get_path(filename, write=True, mkdir=True)
    image.save(filename)


# reads data from a file in the source directory
def file_read(fname, datatype=str):
    if datatype == bytes:
        return file_open(fname, binary=True, write=False).read()
    elif datatype == str:
        return file_open(fname, binary=False, write=False).read()
    else:
        raise Exception("Unknown data type for reading: %s" % str(datatype))


def zstr(data):
    """
    returns the utf8 string representation of a byte array.

    terminates on end of string, or when \0 is reached.
    """

    return data.decode("utf-8").rstrip("\x00")

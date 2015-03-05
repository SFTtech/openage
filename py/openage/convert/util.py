# Copyright 2013-2015 the openage authors. See copying.md for legal info.

import os
import os.path

from openage.log import dbg
from openage.util import mkdirs


def file_write_multi(file_dict, output_folder=False):
    """
    save the given file dictionary to files

    key: filename
    value: file content
    """

    for file_name, file_data in file_dict.items():

        if output_folder:
            file_path = os.path.join(output_folder, file_name)
        else:
            file_path = file_name

        file_folder, _ = os.path.split(file_path)
        mkdirs(file_folder)

        if type(file_data) == bytes:
            mode = "wb"
        else:
            mode = "w"

        dbg("saving %s.." % file_path, 1)
        with open(file_path, mode) as f:
            f.write(file_data)


def zstr(data):
    """
    returns the utf8 string representation of a byte array.

    terminates on end of string, or when \0 is reached.
    """

    end = data.find(0)
    if end != -1:
        data = data[0:end]

    return data.decode("utf-8")

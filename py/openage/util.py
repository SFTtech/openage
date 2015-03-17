# Copyright 2014-2015 the openage authors. See copying.md for legal info.
import os


def mkdirs(path):
    """
    Create directories recursively. Like `mkdir -p`.
    """
    os.makedirs(path, exist_ok=True)


class NamedObject:
    def __init__(self, name, **kw):
        self.name = name
        self.__dict__.update(kw)

    def __repr__(self):
        return self.name


class VirtualFile:
    """
    provides a virtual file, with seek, tell, read and write functions.

    this can be used to store PIL images in particular.
    operates in binary mode.
    """

    def __init__(self, name=None, binary=True):
        self.buf = b"" if binary else ""
        self.name = name
        self.pos = 0

    def seek(self, offset, whence=0):
        raise NotImplementedError("seeking not implemented")

    def tell(self):
        return self.pos

    def read(self):
        return self.buf[self.pos:]

    def write(self, data):
        # we're at the end:
        if self.pos == len(self.buf) - 1:
            self.buf += data
        # we're somewhere in the middle:
        else:
            self.buf = (self.buf[0:self.pos] + data +
                        self.buf[(self.pos + len(data)):])

        self.pos += len(data)

    def data(self):
        return self.buf

    def set_data(self, data):
        self.buf = data
        self.pos = 0

    def __iter__(self):
        lines = self.data().split("\n")

        def get_lines():
            for l in lines:
                yield l
        return get_lines()


def gen_dict_key2lists(keys):
    """
    returns an empty dict with key => list() mapping.
    """

    return dict(
        zip(
            keys,
            ([list() for _ in range(len(keys))])
        )
    )


def ifilename(directory, name):
    """
    returns the case-sensitive filename in a directory
    for a given case-insensitive filename.

    because some operating systems had the great idea to have case insensitive
    filenames, this function will find the correct case for
    a file of any case.
    """

    # get actual file names
    files = os.listdir(directory)

    # create idx->filename dict
    idxmap = dict(enumerate(files))

    # create filename.lower() -> idx dict
    filemap = {v.lower(): k for k, v in idxmap.items()}

    # find the lowercase match that we know
    try:
        idx = filemap[name.lower()]
    except KeyError:
        raise FileNotFoundError(
            "could not find '%s' in '%s' case-insensitively" %
            (name.lower(), directory)
        ) from None

    # and return the whatever-case actual existing filename
    return idxmap[idx]


def ifilepath(basepath, path, test=False):
    """
    returns the actual case-sensitive path for a given
    case-insensitive path.

    basepath must exist case-sensitively of course.
    """

    # normalize path so no ../ are left
    path = os.path.normpath(path)

    # split at /
    parts = path.split(os.path.sep)

    try:
        for p in parts:
            realcase = ifilename(basepath, p)
            basepath = os.path.normpath(os.path.join(basepath, realcase))
    except FileNotFoundError as e:
        if not test:
            raise e from None
        else:
            return False

    return basepath

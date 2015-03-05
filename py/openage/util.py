# Copyright 2014-2015 the openage authors. See copying.md for legal info.


def mkdirs(path):
    """\
    just create these dirs recursively
    """

    import os
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

    def __init__(self):
        self.buf = b""

    def seek(self, destination, whence=0):
        raise NotImplementedError("seek not implemented")

    def tell(self):
        raise NotImplementedError("tell not implemented")

    def read(self):
        raise NotImplementedError("read not implemented")

    def write(self, data):
        self.buf += data

    def data(self):
        return self.buf


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
    """\
    returns the case-sensitive filename in a directory
    for a given case-insensitive filename.

    because some operating systems had the great idea to have case insensitive
    filenames, this function will find the correct case for
    a file of any case.
    """

    import os

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
    """\
    returns the actual case-sensitive path for a given
    case-insensitive path.

    basepath must exist case-sensitively of course.
    """

    import os.path

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

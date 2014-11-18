# Copyright 2014-2014 the openage authors. See copying.md for legal info.


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

import os
import os.path

class NamedObject:
    def __init__(self, name, **kw):
        self.name = name
        self.__dict__.update(kw)

    def __repr__(self):
        return self.name


struct_type_lookup = {
    "char":               "b",
    "unsigned char":      "B",
    "int8_t":             "b",
    "uint8_t":            "B",
    "short":              "h",
    "unsigned short":     "H",
    "int16_t":            "h",
    "uint16_t":           "H",
    "int":                "i",
    "unsigned int":       "I",
    "int32_t":            "i",
    "uint32_t":           "I",
    "long":               "l",
    "unsigned long":      "L",
    "long long":          "q",
    "unsigned long long": "Q",
    "int64_t":            "q",
    "uint64_t":           "Q",
    "float":              "f",
    "double":             "d",
    "char[]":             "s",
}


dbgstack = [(None, 0)]

readpath = "/dev/null"
writepath = "/dev/null"
verbose = 0


def set_verbosity(newlevel):
    global verbose
    verbose = newlevel


def ifdbg(lvl):
    global verbose

    if verbose >= lvl:
        return True
    else:
        return False

def ifdbgeq(lvl):
    global verbose

    if verbose == lvl:
        return True
    else:
        return False


def dbg(msg=None, lvl=None, push=None, pop=None, lazymsg=None, end="\n"):
    """
    msg
        message to print.
        str() will be called on this object.
        if None, the logger state is changed according to the other args,
        but no message is printed.
    lvl
        integer level. if None, the loglevel remains unchanged.
    push
        if not None, will be pushed onto the debug stack.
        the current level is pushed onto the stack as well, for restoring
        on pop.

        messages are indented according to the debug stack length.
    pop
        if not None, the top element of the debug stack will be poped.
        the old loglevel is restored.
        an exception is generated if the object is not equal to the top
        object on the debug stack.
    lazymsg
        can be used instead of msg for lazy evaluation.
        must be callable, and return a message object (or None).
        will be called only if the message will be actually printed (according to loglevel).
        intended for use with lambdas.
        only one of msg and lazymsg may be not None.
    end
        the 'end' argument to print
    """

    global verbose

    if lvl == None:
        #if no level is set, use the level on top of the debug stack
        lvl = dbgstack[-1][1]

    if pop != None:
        if pop == True:
            if dbgstack.pop()[0] == None:
                raise Exception("stack underflow in debug stack!")
        elif dbgstack.pop()[0] != pop:
            raise Exception(str(pop) + " is not on top of the debug stack")

    if verbose >= lvl:
        if lazymsg != None:
            if callable(lazymsg):
                msg = lazymsg()
            else:
                raise Exception("the lazy message must be a callable (lambda)")

        if msg != None:
            print((len(dbgstack) - 1) * "  " + str(msg), end = end)

    if push != None:
        dbgstack.append((push, lvl))


def mkdirs(path):
    os.makedirs(path, exist_ok = True)


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


def file_open(path, binary = True, write = False):
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

    #ensure that the directory exists
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


#reads data from a file in the source directory
def file_read(fname, datatype = str):
    if datatype == bytes:
        return file_open(fname, binary=True, write=False).read()
    elif datatype == str:
        return file_open(fname, binary=False, write=False).read()
    else:
        raise Exception("Unknown data type for reading: " + str(datatype))


def zstr(data):
    """
    returns the utf8 string representation of a byte array.

    terminates on end of string, or when \0 is reached.
    """

    return data.decode("utf-8").rstrip("\x00")


def offset_info(offset, data, msg="", s=None, mode=0):
    ret = "====== @ %d = %#x ======" % (offset, offset)
    ret += " %s " % msg

    #print struct info
    if s != None:
        ret += "== \"" + str(s.format.decode("utf-8")) + "\" =="

    #long info mode
    if mode == 0:
        ret += "\n" + str(data) + "\n"
    elif mode == 1:
        ret += " " + str(data)

    dbg(ret, 3)


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

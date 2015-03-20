# Copyright 2015-2015 the openage authors. See copying.md for legal info.
"""
Some file handling utilities
"""


def read_guaranteed(fileobj, size):
    """
    As regular fileobj.read(size), but raises EOFError if fewer bytes
    than requested are returned.
    """
    data = fileobj.read(size)

    if len(data) != size:
        raise EOFError()

    return data


def read_nullterminated_string(fileobj, maxlen=255):
    """
    Reads bytes until a null terminator is reached.
    """
    result = bytearray()

    while True:
        try:
            char = read_guaranteed(fileobj, 1)[0]
        except IndexError:
            raise EOFError("EOF while reading null-terminated string: " +
                           repr(b"".join(result))) from None

        if char == 0:
            return bytes(result)
        else:
            result.append(char)

        if len(result) > maxlen:
            raise Exception("Null-terminated string too long.")

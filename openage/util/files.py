# Copyright 2015-2015 the openage authors. See copying.md for legal info.
"""
Some file handling utilities
"""


def read_guaranteed(fileobj, size):
    """
    As regular fileobj.read(size), but raises EOFError if fewer bytes
    than requested are returned.
    """
    remaining = size
    result = []

    while remaining:
        data = fileobj.read(remaining)
        if not data:
            raise EOFError()

        remaining -= len(data)
        result.append(data)

    return b"".join(result)


def read_nullterminated_string(fileobj, maxlen=255):
    """
    Reads bytes until a null terminator is reached.
    """
    result = bytearray()

    while True:
        char = ord(read_guaranteed(fileobj, 1))

        if char == 0:
            return bytes(result)
        else:
            result.append(char)

        if len(result) > maxlen:
            raise Exception("Null-terminated string too long.")

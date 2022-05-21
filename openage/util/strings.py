# Copyright 2015-2022 the openage authors. See copying.md for legal info.
"""
Misc string helper functions; this includes encoding, decoding,
manipulation, ...
"""

from sys import stdout


def decode_until_null(data: bytes, encoding: str = 'utf-8') -> str:
    """
    decodes a bytes object, aborting at the first \\0 character.

    >>> decode_until_null(b"foo\\0bar")
    'foo'
    """
    end = data.find(0)
    if end != -1:
        data = data[:end]

    return data.decode(encoding)


def try_decode(data: bytes) -> str:
    """
    does its best to attempt decoding the given string of unknown encoding.
    """
    try:
        return data.decode('utf-8')
    except UnicodeDecodeError:
        pass

    return data.decode('iso-8859-1')


def binstr(num: int, bits: int = None, group: int = 8) -> str:
    """
    Similar to the built-in bin(), but optionally takes
    the number of bits as an argument, and prints underscores instead of
    zeroes.

    >>> binstr(1337, 16)
    '_____1_1 __111__1'
    """
    result = bin(num)[2:]

    if bits is not None:
        result = result.rjust(bits, '0')

    result = result.replace('0', '_')

    if group is not None:
        grouped = [result[i:i + group] for i in range(0, len(result), group)]
        result = ' '.join(grouped)

    return result


def colorize(string: str, colorcode: str) -> str:
    """
    Colorizes string with the given EMCA-48 SGR code.

    >>> colorize('foo', '31;1')
    '\\x1b[31;1mfoo\\x1b[m'
    """
    if colorcode:
        colorized = f'\x1b[{colorcode}m{string}\x1b[m'
    else:
        colorized = string

    return colorized


def lstrip_once(string: str, substr: str) -> str:
    """
    Removes substr at the start of string, and raises ValueError on failure.

    >>> lstrip_once("openage.test", "openage.")
    'test'
    >>> lstrip_once("libopenage.test", "openage.")
    Traceback (most recent call last):
    ValueError: 'libopenage.test' doesn't start with 'openage.'
    """
    if not string.startswith(substr):
        raise ValueError(f"{repr(string)} doesn't start with {repr(substr)}")

    return string[len(substr):]


def rstrip_once(string: str, substr: str) -> str:
    """
    Removes substr at the end of string, and raises ValueError on failure.

    >>> rstrip_once("test.cpp", ".cpp")
    'test'
    """
    if not string.endswith(substr):
        raise ValueError(
            f"{repr(string)} doesn't end with {repr(substr)}")

    return string[:-len(substr)]


def format_progress(progress: int, total: int) -> str:
    """
    Formats an "x out of y" string with fixed width.

    >>> format_progress(5, 20)
    ' 5/20'
    """
    return f"{progress:>{len(str(total))}}/{total}"


def print_progress(progress: int, total: int) -> str:
    """
    Print an "x out of y" string with fixed width to stdout.
    The output overwrites itself.
    """
    stdout.write(format_progress(progress, total) + "\r")

# Copyright 2014-2018 the openage authors. See copying.md for legal info.

"""
Some utilities.
"""

import os


SHEBANG = "#!/.*\n(#?\n)?"

FILECACHE = {}
BADUTF8FILES = set()


def has_ext(fname, exts):
    """
    Returns true if fname ends in any of the extensions in ext.
    """
    for ext in exts:
        if ext == '':
            if os.path.splitext(fname)[1] == '':
                return True
        elif fname.endswith(ext):
            return True

    return False


def readfile(filename):
    """
    reads the file, and returns it as a str object.

    if the file has already been read in the past,
    returns it from the cache.
    """
    if filename not in FILECACHE:
        with open(filename, 'rb') as fileobj:
            data = fileobj.read()

        try:
            data = data.decode('utf-8')
        except UnicodeDecodeError:
            data = data.decode('utf-8', errors='replace')
            BADUTF8FILES.add(filename)

        FILECACHE[filename] = data

    return FILECACHE[filename]


def writefile(filename, new_content):
    """
    writes the file and update it in the cache.
    """
    if filename in BADUTF8FILES:
        raise Exception(f"{filename}: cannot write due to utf8-errors.")

    with open(filename, 'w') as fileobj:
        fileobj.write(new_content)

    FILECACHE[filename] = new_content


def findfiles(paths, exts=None):
    """
    yields all files in paths with names ending in an ext from exts.

    If exts is None, all extensions are accepted.

    hidden dirs and files are ignored.
    """
    for path in paths:
        for filename in os.listdir(path):
            if filename.startswith('.'):
                continue

            filename = os.path.join(path, filename)

            if os.path.isdir(filename):
                yield from findfiles((filename,), exts)
                continue

            if exts is None or has_ext(filename, exts):
                yield filename


def issue_str(title, filename, fix=None):
    """
    Creates a formated (title, text) desciption of an issue.

    TODO use this function and issue_str_line for all issues, so the format
    can be easily changed (exta text, colors, etc)
    """
    return (title, filename, fix)


# gread hint, pylint. thank you so much.
# pylint: disable=too-many-arguments
def issue_str_line(title, filename, line, line_number, highlight, fix=None):
    """
    Creates a formated (title, text) desciption of an issue with information
    about the location in the file.
    line:        line content
    line_number: line id in the file
    highlight:   a tuple of (start, end), where
        start:   match start in the line
        end:     match end in the line
    """

    start, end = highlight
    start += 1
    line = line.replace("\n", "").replace("\t", " ")

    return (
        title,
        (
            filename + "\n"
            "\tline: " + str(line_number) + "\n"   # line number
            "\tat:   '" + line + "'\n"             # line content
            "\t      " + (' ' * start) +           # mark position with ^
            "\x1b[32;1m^" + ("~" * (end - start)) + "\x1b[m"
        ),
        fix
    )

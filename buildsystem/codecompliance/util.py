# Copyright 2014-2023 the openage authors. See copying.md for legal info.

"""
Some utilities.
"""

import logging
from collections.abc import Iterable, Iterator
from pathlib import Path

SHEBANG = "#!/.*\n(#?\n)?"

FILECACHE = {}
BADUTF8FILES = set()


def log_setup(setting, default=1):
    """
    Perform setup for the logger.
    Run before any logging.log thingy is called.

    if setting is 0: the default is used, which is WARNING.
    else: setting + default is used.
    """

    levels = (logging.ERROR, logging.WARNING, logging.INFO, logging.DEBUG, logging.NOTSET)

    factor = clamp(default + setting, 0, len(levels) - 1)
    level = levels[factor]

    logging.basicConfig(level=level, format="[%(asctime)s] %(message)s")
    logging.captureWarnings(True)


def clamp(number, smallest, largest):
    """return number but limit it to the inclusive given value range"""
    return max(smallest, min(number, largest))


class Strlazy:
    # pylint: disable=too-few-public-methods
    """
    to be used like this: logging.debug("rolf %s", strlazy(lambda: do_something()))
    so do_something is only called when the debug message is actually printed
    do_something could also be an f-string.
    """

    def __init__(self, fun):
        self.fun = fun

    def __str__(self):
        return self.fun()


def has_ext(fname: Path, exts: Iterable[str]) -> bool:
    """
    Returns true if fname ends in any of the extensions in ext.
    """
    for ext in exts:
        if ext == "":
            if fname.suffix == "":
                return True
        elif fname.name.endswith(ext):
            return True

    return False


def readfile(filename: Path) -> str:
    """
    reads the file, and returns it as a str object.

    if the file has already been read in the past,
    returns it from the cache.
    """
    if filename not in FILECACHE:
        with open(filename, "rb") as fileobj:
            data = fileobj.read()

        try:
            data = data.decode("utf-8")
        except UnicodeDecodeError:
            data = data.decode("utf-8", errors="replace")
            BADUTF8FILES.add(filename)

        FILECACHE[filename] = data

    return FILECACHE[filename]


def writefile(filename: Path, new_content: str) -> None:
    """
    writes the file and update it in the cache.
    """
    if filename in BADUTF8FILES:
        raise ValueError(f"{filename}: cannot write due to utf8-errors.")

    with open(filename, "w", encoding="utf8") as fileobj:
        fileobj.write(new_content)

    FILECACHE[filename] = new_content


def findfiles(paths: Iterable[Path], exts: Iterable[str] | None = None) -> Iterator[Path]:
    """
    yields all files in paths with names ending in an ext from exts.

    A path that is a file is yielded as-is: the extension filter only
    applies to files found under a directory path.
    If exts is None, all extensions are accepted.

    hidden dirs and files are ignored.
    """
    for path in paths:
        if not path.is_dir():
            yield path
            continue

        for filename in path.iterdir():
            if filename.name.startswith("."):
                continue

            if filename.is_dir():
                yield from findfiles((filename,), exts)
                continue

            if exts is None or has_ext(filename, exts):
                yield filename


def select_files(
    check_files: Iterable[Path] | None,
    locations: Iterable[Path],
    exts: Iterable[str] | None = None,
) -> Iterator[Path]:
    """
    Yields the files to check from the given locations.

    A location that is a directory is searched for files ending in an ext
    from exts (all files if exts is None); a location that is a direct file
    is always used.

    If check_files is given, only files from that set are yielded: those
    that are one of the locations, or lie under one of them and match exts.
    """
    if check_files is None:
        yield from findfiles(locations, exts)
        return

    for path in check_files:
        if path in locations:
            yield path
        elif any(path.is_relative_to(location) for location in locations):
            if exts is None or has_ext(path, exts):
                yield path


def issue_str(title: str, filename: Path, fix=None) -> tuple[str, Path, None]:
    """
    Creates a formated (title, text) desciption of an issue.

    TODO use this function and issue_str_line for all issues, so the format
    can be easily changed (exta text, colors, etc)
    """
    return (title, filename, fix)


# gread hint, pylint. thank you so much.
def issue_str_line(
    title: str,
    filename: Path,
    line: str,
    line_number: int,
    highlight: tuple[int, int],
    fix=None,
) -> tuple[str, str, None]:
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
            f"{filename}\n"
            "\tline: " + str(line_number) + "\n"  # line number
            "\tat:   '" + line + "'\n"  # line content
            "\t      "
            + (" " * start)  # mark position with ^
            + "\x1b[32;1m^"
            + ("~" * (end - start))
            + "\x1b[m"
        ),
        fix,
    )

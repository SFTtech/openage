# Copyright 2015-2015 the openage authors. See copying.md for legal info.
"""
Downloads the SFT test cab archive and uses it to test the cabextract code.
"""

import os
from tempfile import gettempdir
from hashlib import md5
from urllib.request import urlopen

from ..testing.testing import assert_result, assert_raises

from .cab import CABFile

# the test archive file has been generated using ./gen_test_arc.sh

# filesize of the test archive file
TEST_ARCHIVE_SIZE = 1057766

# URL of the test archive file
TEST_ARCHIVE_URL = "http://pub.sft.mx/openage/openage_testarc.cab"

# list of all files in the test archive, with (md5sum, size).
TEST_FILES = {
    "testfilea": ("68fe7d874c7887bdd3f6aaca46abd306", 524288),
    "testfileb": ("7f614da9329cd3aebf59b91aadc30bf0", 67108864),
    "testfilec": ("d41d8cd98f00b204e9800998ecf8427e", 0),
    "testdir/testfiled": ("6f75302850b485421030f034fe67380b", 10),
    "testdir/testfilee": ("65116b303f205837d280d3c08b1e0419", 1033720)
}

# local cache filename
TEST_ARCHIVE_FILENAME = os.path.join(gettempdir(), "openage_testarc.cab")


def open_cached_test_archive():
    """
    Opens the cached test archive file.
    """
    if os.path.getsize(TEST_ARCHIVE_FILENAME) != TEST_ARCHIVE_SIZE:
        raise OSError("test archive has wrong size")

    return open(TEST_ARCHIVE_FILENAME, 'rb')


def open_test_archive():
    """
    Opens the cached test archive file, or downloads it if necessary.
    """
    try:
        return open_cached_test_archive()
    except OSError:
        pass

    with open(TEST_ARCHIVE_FILENAME, 'wb') as test_arc:
        test_arc.write(urlopen(TEST_ARCHIVE_URL).read())

    return open_cached_test_archive()


def test():
    """
    The actual test function; registered in openage.testing.testlist.
    """
    # acquire the actual test archive file and create the CABFile Path object
    cab = CABFile(open_test_archive()).root

    testdir = cab["..////./../testdir"]
    nonexistingdir = cab["nonexistingdir"]
    nonexistingfile = cab["nonexistingfile"]
    testfilea = cab["testfilea"]

    # list dir
    assert_result(
        lambda: sorted(path.name for path in cab.iterdir()),
        ["testdir", "testfilea", "testfileb", "testfilec"]
    )

    # list subdir
    assert_result(
        lambda: sorted(path.name for path in testdir.iterdir()),
        ["testfiled", "testfilee"]
    )

    # list nonexisting dir
    assert_raises(
        lambda: tuple(nonexistingdir.iterdir()),
        FileNotFoundError
    )

    # filesize for nonexisting file
    assert_raises(
        lambda: nonexistingfile.filesize,
        FileNotFoundError
    )

    # filesize for dir
    assert_raises(
        lambda: testdir.filesize,
        IsADirectoryError
    )

    # mtime for a non-existing file
    assert_raises(
        lambda: nonexistingfile.mtime,
        FileNotFoundError
    )

    # attempt getting mtime for a directory
    assert_raises(
        lambda: testdir.mtime,
        IsADirectoryError
    )

    # attempt getting mtime for a file
    assert_result(
        lambda: testfilea.mtime,
        1430844692
    )

    # open nonexisting file
    assert_raises(
        lambda: nonexistingfile.open('rb'),
        FileNotFoundError
    )

    # open existing file
    testfiled = assert_result(
        lambda: cab.joinpath('///testdir//.//testfiled').open('rb'),
        validator=bool
    )

    # seek around and read a bit
    assert_result(
        lambda: testfiled.seek(3),
        None
    )

    assert_result(
        lambda: testfiled.read(4),
        b"tfil"
    )

    assert_result(
        lambda: testfiled.seek(-1, os.SEEK_CUR),
        None
    )

    assert_result(
        testfiled.tell,
        6
    )

    assert_raises(
        lambda: testfiled.seek(-8, os.SEEK_CUR),
        ValueError
    )

    assert_result(
        lambda: testfiled.seek(-4, os.SEEK_END),
        None
    )

    assert_result(
        testfiled.tell,
        6
    )

    assert_result(
        lambda: testfiled.read(5),
        b"led\n"
    )

    # test filesize and md5sum for all files
    for filename, (md5sum, size) in TEST_FILES.items():
        assert_result(
            lambda name=filename:
            md5(cab.joinpath(name).open('rb').read()).hexdigest(),
            md5sum
        )

        assert_result(
            lambda name=filename: cab.joinpath(name).filesize,
            size
        )

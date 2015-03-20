# Copyright 2015-2015 the openage authors. See copying.md for legal info.
"""
Downloads the SFT test cab archive and uses it to test the cabextract code.
"""

import os
from io import UnsupportedOperation
from tempfile import gettempdir
from hashlib import md5
from urllib.request import urlopen

from ..testing.testing import assert_result, assert_raises

from .cab import CABFile

# the test archive file has been generated using ./gen_test_arc.sh

# filesize of the test archive file
TEST_ARCHIVE_SIZE = 1057766

# URL of the test archive file
TEST_ARCHIVE_URL = "http://pub.sft.mx/openage_testarc.cab"

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
    # acquire the actual test archive file
    cabfileobj = open_test_archive()

    # create the CABFile object
    cabfile = CABFile(cabfileobj)

    # list dirs in dir
    assert_result(
        lambda: tuple(cabfile.listdirs()),
        ("testdir",))

    # list files in dir
    assert_result(
        lambda: tuple(cabfile.listfiles()),
        ("testfilea", "testfileb", "testfilec"))

    # list dirs in empty dir
    assert_result(
        lambda: tuple(cabfile.listdirs('///./testdir')),
        tuple())

    # list dirs in nonexisting dir
    assert_raises(
        lambda: tuple(cabfile.listdirs('nonexistingdir')),
        FileNotFoundError)

    # list files in nonexisting dir
    assert_raises(
        lambda: tuple(cabfile.listfiles('nonexistingdir')),
        FileNotFoundError)

    # filesize for nonexisting file
    assert_raises(
        lambda: cabfile.filesize('nonexistingfile'),
        FileNotFoundError)

    # filesize for dir
    assert_raises(
        lambda: cabfile.filesize('testdir'),
        FileNotFoundError)

    # attempt getting mtime for a non-existing file
    assert_raises(
        lambda: cabfile.mtime('nonexistingfile'),
        FileNotFoundError)

    # attempt getting mtime for a directory
    assert_raises(
        lambda: cabfile.mtime('testdir'),
        UnsupportedOperation)

    # attempt getting mtime for a file
    assert_result(
        lambda: cabfile.mtime('testfilea'),
        1430844692)

    # open file as non-'rb'
    assert_raises(
        lambda: cabfile.open('testdir/testfiled'),
        UnsupportedOperation)

    # open nonexisting file
    assert_raises(
        lambda: cabfile.open('nonexistingfile', 'rb'),
        FileNotFoundError)

    # open existing file
    testfiled = assert_result(
        lambda: cabfile.open('///testdir//.//testfiled', 'rb'),
        validator=bool)

    # seek around and read a bit
    assert_result(
        lambda: testfiled.seek(3),
        None)

    assert_result(
        lambda: testfiled.read(4),
        b"tfil")

    assert_result(
        lambda: testfiled.seek(-1, os.SEEK_CUR),
        None)

    assert_result(
        testfiled.tell,
        6)

    assert_raises(
        lambda: testfiled.seek(-8, os.SEEK_CUR),
        ValueError)

    assert_result(
        lambda: testfiled.seek(-4, os.SEEK_END),
        None)

    assert_result(
        testfiled.tell,
        6)

    assert_result(
        lambda: testfiled.read(5),
        b"led\n")

    # test filesizes and md5sum for all files
    for filename, (md5sum, size) in TEST_FILES.items():
        assert_result(
            lambda name=filename:
            md5(cabfile.open(name, 'rb').read()).hexdigest(),
            md5sum)

        assert_result(
            lambda name=filename: cabfile.filesize(name),
            size)

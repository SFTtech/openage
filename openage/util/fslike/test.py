# Copyright 2017-2017 the openage authors. See copying.md for legal info.
"""
Tests for the filesystem-like abstraction.
"""

import os

from io import UnsupportedOperation
from tempfile import gettempdir, NamedTemporaryFile

from openage.testing.testing import assert_value, assert_raises, result

from .directory import Directory, CaseIgnoringDirectory
from .union import Union
from .wrapper import WriteBlocker, DirectoryCreator


def test_path(root_path, root_dir):
    """
    Test basic functionality of fslike.Path
    """

    # multi dir creation
    deeper = (root_path / "let's go" / "deeper")
    assert_value(deeper.parent, root_path["let's go"])
    deeper.mkdirs()
    assert_value(deeper.is_dir(), True)
    assert_value(deeper.resolve_native_path().decode(),
                 os.path.join(root_dir, "let's go", "deeper"))

    insert = deeper["insertion.stuff.test"]
    insert.touch()
    assert_value(insert.filesize, 0)
    assert_value(insert.suffix, ".test")
    assert_value(insert.suffixes, [".stuff", ".test"])
    assert_value(insert.stem, "insertion.stuff")
    assert_value(insert.with_name("insertion.stuff.test").exists(), True)
    assert_value(insert.with_suffix(".test").exists(), True)

    root_path["let's go"].removerecursive()


def test_union(root_path, root_dir):
    """
    Union functionality testing.

    Procedure:
    create and write a file in r
    create union with w and r mount. r is readonly.
    read file, should be from r.
    write file, whould go to w.
    read file, should be from w.
    unmount w, file content should be from r again.
    unmount r, union should be empty now.
    """

    test_dir_w = os.path.join(root_dir, "w")
    test_dir_r = os.path.join(root_dir, "r")

    # automated directory creation:
    path_w = DirectoryCreator(
        Directory(test_dir_w, create_if_missing=True).root
    ).root
    path_r = Directory(test_dir_r, create_if_missing=True).root

    assert_value(path_r["some_file"].is_file(), False)

    with path_r["some_file"].open("wb") as fil:
        fil.write(b"some data")

    with path_r["some_file"].open("rb") as fil:
        assert_value(b"some data", fil.read())

    assert_value(path_r.exists(), True)
    assert_value(path_r.is_dir(), True)
    assert_value(path_r.is_file(), False)
    assert_value(path_r["some_file"].is_file(), True)
    assert_value(path_r.writable(), True)

    # protect the r-path
    path_protected = WriteBlocker(path_r).root
    assert_value(path_protected.writable(), False)

    with assert_raises(UnsupportedOperation):
        result(path_protected.open('wb'))

    # mount the above into one virtual file system
    target = Union().root

    # first, mount the read-directory read-only
    target.mount(path_protected)

    # then, mount the writable folder
    target.mount(path_w)

    # read the data
    with target["some_file"].open("rb") as fil:
        test_data = fil.read()

    # overwrite the data:
    with target["some_file"].open("wb") as fil:
        fil.write(b"we changed it")

    # get back changed data
    with target["some_file"].open("rb") as fil:
        changed_test_data = fil.read()

    assert_value(test_data != changed_test_data, True)
    assert_value(changed_test_data, b"we changed it")

    # ther should be nothing else here.
    assert_value(set(root_path.list()), {b"r", b"w"})

    # unmount the change-overlay
    target.unmount(path_w)

    with (target / "some_file").open("rb") as fil:
        unchanged_test_data = fil.read()

    assert_value(test_data, unchanged_test_data)

    # unmount the source-overlay:
    target.unmount()

    # now the target mount should be completely empty.
    assert_value(target["some_file"].exists(), False)

    assert_value(list(target.list()), [])
    assert_value(len(list(target.iterdir())), 0)


def is_filesystem_case_sensitive():
    """
    Utility function to verify if filesystem is case-sensitive.
    """

    with NamedTemporaryFile() as tmpf:
        # we now have a file with a "tmp" prefixed name
        # if it exists in upper case also, filesystem is not case-sensitive
        return not os.path.exists(tmpf.name.upper())


def test_case_ignoring(root_path, root_dir):
    """
    Test the case ignoring directory,
    which mimics the windows filename selection behavior.
    """

    # create a file with known name
    with root_path["lemme_in"].open("wb") as fil:
        fil.write(b"pwnt")

    ignorecase_dir = CaseIgnoringDirectory(root_dir).root

    # open it with wrong-case name
    with ignorecase_dir["LeMmE_In"].open("rb") as fil:
        assert_value(fil.read(), b"pwnt")

    # then write it with wrong-case name
    with ignorecase_dir["LeMmE_In"].open("wb") as fil:
        fil.write(b"changedit")

    # check if changes went to known-name file
    with root_path["lemme_in"].open("rb") as fil:
        assert_value(fil.read(), b"changedit")

    # create new file with CamelCase name
    ignorecase_dir["WeirdCase"].touch()

    # check if the CamelCase file was actually created as `camelcase`
    assert_value(root_path["weirdcase"].is_file(), True)

    # touching the same file
    root_path["a"].touch()
    ignorecase_dir["A"].touch()

    if is_filesystem_case_sensitive():
        # 'A' should not exist, 'a' should have been touched.
        assert_value(root_path["A"].is_file(), False)
    else:
        # The underlying fs should treat A as a.
        assert_value(root_path["A"].is_file(), True)


def test():
    """
    Perform functionality tests for the filesystem abstraction interface.
    """

    # create a clean test folder in /tmp
    root_dir = os.path.join(gettempdir(), "openage_fslike_test")
    root_path = Directory(root_dir, create_if_missing=True).root
    root_path.removerecursive()

    # test basic path functions
    test_path(root_path, root_dir)

    # test the union
    test_union(root_path, root_dir)

    # test the case ignoring dir
    test_case_ignoring(root_path, root_dir)

    # and remove all the things we just created
    assert_value(root_path.is_dir(), True)
    root_path.removerecursive()
    assert_value(root_path.is_dir(), False)

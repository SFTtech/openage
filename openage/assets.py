# Copyright 2014-2015 the openage authors. See copying.md for legal info.

"""
Code for locating assets.
"""

# TODO: pyinterface this; integrate util.fslike.FileSystemLikeObject

# TODO pylint: disable=C

# TODO just about the entire file.

import os
from . import config

# the python root directory (the one directory from sys.path that contains
# the currently loaded openage modules)
import openage
python_root = os.path.dirname(os.path.dirname(openage.__file__))

# the development directory, IF the python module is run from a development
# directory.
sourcedir_root = python_root


def is_in_devmode():
    return True
    # very much TODO

    # import importlib
    # try:
    #     importlib.import_module("..buildsystem.setup")
    #     return True
    # except ImportError:
    #     return False
if os.path.isfile(os.path.join(sourcedir_root, 'openage_version')):
    # the openage_version file exists; we're in the dev dir
    is_installed = False
else:
    # the openage_version file doesn't exist; we're installed
    is_installed = True


if is_in_devmode():
    # TODO cross-platform code
    global_dir = os.path.join(sourcedir_root, 'assets')
    user_dir = os.path.join(sourcedir_root, 'userassets')
else:
    global_dir = config.GLOBAL_ASSET_DIR
    user_dir = os.path.join(os.environ['HOME'], '.openage')


def _global_path(asset):
    """
    returns the path to the given asset, in the global asset dir
    """
    return os.path.join(global_dir, asset)


def _user_path(asset):
    """
    returns the path to the given asset, in the user asset dir
    """
    return os.path.join(user_dir, asset)


def _dir_entries(path, files=True, dirs=False):
    """
    returns all filenames/dirnames in a directory
    """
    if os.path.isfile(path):
        raise NotADirectoryError("not a directory: %s" % path)
    if os.path.isdir(path):
        for f in os.listdir(path):
            if os.path.isfile(f):
                if files:
                    yield f
            else:
                if dirs:
                    yield f


def _list_assetdir(assetpath, files=True, dirs=False):
    """
    returns all file and/or directory assets in assetpath
    """
    results = set()
    for f in _dir_entries(_user_path(assetpath), files=files, dirs=dirs):
        results.add(f)
    for f in _dir_entries(_global_path(assetpath), files=files, dirs=dirs):
        results.add(f)

    return sorted(results)


def list_files(assetpath):
    """
    lists all file assets in assetpath
    """
    return _list_assetdir(assetpath, files=True, dirs=False)


def list_dirs(assetpath):
    """
    lists all directory assets in assetpath
    """
    return _dir_entries(assetpath, files=False, dirs=True)


def get_file(asset, writable=False):
    """
    gets filename for asset

    asset:
        asset path
    writable:
        do we want a readable or a writable file
        if True, a filename in the user asset dir is returned,
        and directories are created if neccesary.
        if False, the file must exist; the user asset file is preferred
        over the global asset file.
    """
    if writable:
        path = _user_path(asset)
        if os.path.isdir(path):
            raise IsADirectoryError("Asset is not a file: %s" % asset)

        dirname, _ = os.path.split(path)
        if not os.path.isdir(dirname):
            os.makedirs(dirname)

        return path
    else:
        path = _user_path(asset)
        if os.path.isdir(path):
            raise IsADirectoryError("Asset is not a file: %s" % asset)
        if not os.path.isfile(path):
            path = _global_path(asset)
        if os.path.isdir(path):
            raise IsADirectoryError("Asset is not a file: %s" % asset)
        if not os.path.isfile(path):
            raise FileNotFoundError("Asset not found: %s" % asset)

        return path


def test():
    from .testing.testing import assert_result
    assert_result(lambda: os.path.isdir(global_dir), True)

import os
from . import config

# the python root directory (the one directory from sys.path that contains
# the currently loaded openage modules)
import openage
python_root = os.path.dirname(os.path.dirname(openage.__file__))

# the development directory, IF the python module is run from a development
# directory.
sourcedir_root = os.path.dirname(python_root)

if os.path.isfile(os.path.join(sourcedir_root, 'openage_version')):
    # the openage_version file exists; we're in the dev dir
    is_installed = False
else:
    # the openage_version file doesn't exist; we're installed
    is_installed = True


if is_installed:
    # TODO cross-platform code
    global_asset_dir = config.global_asset_dir
    user_asset_dir = os.path.join(os.environ['HOME'], '.openage')
else:
    global_asset_dir = os.path.join(sourcedir_root, 'assets')
    user_asset_dir = os.path.join(sourcedir_root, 'userassets')


def global_asset_path(asset):
    """
    returns the path to the given asset, in the global asset dir
    """
    return os.path.join(global_asset_dir, asset)


def user_asset_path(asset):
    """
    returns the path to the given asset, in the user asset dir
    """
    return os.path.join(user_asset_dir, asset)


def list_dir(path, files=True, dirs=False):
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


def list_assets(path, files=True, dirs=False):
    results = set()
    for f in list_dir(user_asset_path(assetpath), files=files, dirs=dirs):
        results.add(f)
    for f in list_dir(global_asset_path(assetpath), files=files, dirs=dirs):
        results.add(f)

    return sorted(results)


def get_asset_file(asset):
    path = user_asset_path(asset)
    if os.path.isdir(path):
        raise IsADirectoryError("Asset is not a file: %s" % asset)
    if not os.path.isfile(path):
        path = global_asset_path(asset)
    if os.path.isdir(path):
        raise IsADirectoryError("Asset is not a file: %s" % asset)
    if not os.path.isfile(path):
        raise FileNotFoundError("Asset not found: %s" % asset)

    return path


def get_writable_asset_file(asset):
    path = user_asset_path(asset)
    if os.path.isdir(path):
        raise IsADirectoryError("Asset is not a file: %s" % asset)

    dirname, _ = os.path.split(path)
    if not os.path.isdir(dirname):
        os.makedirs(dirname)

    return path


def test():
    if not os.path.isdir(global_asset_dir):
        raise Exception("Asset directory not found: %s" % global_asset_dir)

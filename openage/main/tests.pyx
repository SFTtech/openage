# Copyright 2018-2018 the openage authors. See copying.md for legal info.

"""
tests for the engine itself.
"""

import argparse

from libopenage.util.path cimport Path as Path_cpp
from libopenage.pyinterface.pyobject cimport PyObj
from cpython.ref cimport PyObject
from libopenage.main.tests cimport engine_demo as engine_demo_c


def engine_demo(list argv):
    """
    Invokes the available engine demos.
    """

    cmd = argparse.ArgumentParser(
        prog='... engine_demo',
        description='Demo of the game engine features')
    cmd.add_argument("test_id", type=int, help="id of the demo to run.")
    cmd.add_argument("--asset-dir",
                     help="Use this as an additional asset directory.")
    cmd.add_argument("--cfg-dir",
                     help="Use this as an additional config directory.")

    args = cmd.parse_args(argv)


    from ..cvar.location import get_config_path
    from ..assets import get_asset_path
    from ..util.fslike.union import Union

    # create virtual file system for data paths
    root = Union().root

    # mount the assets folder union at "assets/"
    root["assets"].mount(get_asset_path(args.asset_dir))

    # mount the config folder at "cfg/"
    root["cfg"].mount(get_config_path(args.cfg_dir))

    cdef int engine_test_id = args.test_id

    cdef Path_cpp root_cpp = Path_cpp(PyObj(<PyObject*>root.fsobj),
                                      root.parts)

    with nogil:
        engine_demo_c(engine_test_id, root_cpp)

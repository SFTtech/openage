# Copyright 2023-2023 the openage authors. See copying.md for legal info.

"""
tests for the games simulation.
"""

import argparse

from libopenage.util.path cimport Path as Path_cpp
from libopenage.pyinterface.pyobject cimport PyObj
from cpython.ref cimport PyObject
from libopenage.gamestate.demo.tests cimport simulation_demo as simulation_demo_c


def simulation_demo(list argv):
    """
    invokes the available simulation demos.
    """

    cmd = argparse.ArgumentParser(
        prog='... simulation_demo',
        description='Demo of the game simulation')
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

    cdef int simulation_test_id = args.test_id

    cdef Path_cpp root_cpp = Path_cpp(PyObj(<PyObject*>root.fsobj),
                                  root.parts)

    with nogil:
        simulation_demo_c(simulation_test_id, root_cpp)

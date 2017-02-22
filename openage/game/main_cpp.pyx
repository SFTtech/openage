# Copyright 2015-2017 the openage authors. See copying.md for legal info.

from cpython.ref cimport PyObject
from libcpp.memory cimport make_unique
from libcpp.string cimport string
from libcpp.vector cimport vector

from libopenage.main cimport main_arguments, run_game as run_game_cpp
from libopenage.util.path cimport Path as Path_cpp
from libopenage.pyinterface.pyobject cimport PyObjectRef


cdef extern from "Python.h":
    void PyEval_InitThreads()


def run_game(args, assets):
    """
    Lauches the game after arguments were translated.
    """

    # argument translation
    cdef main_arguments args_cpp

    # assets is a util.fslike.Path object from python
    # we unwrap the filesystem-like pyobject and the path parts
    cdef vector[string] parts = assets.parts

    cdef PyObjectRef ref = PyObjectRef(<PyObject*>assets.fsobj)

    # and create a matching cpp object
    args_cpp.asset_path = make_unique[Path_cpp](ref, parts)

    # frame limiting
    if args.fps is not None:
        args_cpp.fps_limit = args.fps
    else:
        args_cpp.fps_limit = 0

    # opengl debugging
    args_cpp.gl_debug = args.gl_debug

    # create the gil, because now starts the multithread part!
    PyEval_InitThreads()

    # run the game!
    cdef int result
    with nogil:
        result = run_game_cpp(args_cpp)

    return result

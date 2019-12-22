# Copyright 2015-2018 the openage authors. See copying.md for legal info.

from cpython.ref cimport PyObject
from libcpp.memory cimport make_unique
from libcpp.string cimport string
from libcpp.vector cimport vector

from libopenage.main cimport main_arguments, run_game as run_game_cpp
from libopenage.util.path cimport Path as Path_cpp
from libopenage.pyinterface.pyobject cimport PyObj
from libopenage.error.handlers cimport set_exit_ok


cdef extern from "Python.h":
    void PyEval_InitThreads()


def run_game(args, root_path):
    """
    Launches the game after arguments were translated.
    """
    cdef int result

    # argument translation
    cdef main_arguments args_cpp

    set_exit_ok(False)
    try:
        # root_path is a util.fslike.Path object from python
        args_cpp.root_path = Path_cpp(PyObj(<PyObject*>root_path.fsobj),
                                    root_path.parts)

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
        with nogil:
            result = run_game_cpp(args_cpp)

        return result
    finally:
        set_exit_ok(True)

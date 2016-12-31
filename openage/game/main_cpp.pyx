# Copyright 2015-2016 the openage authors. See copying.md for legal info.


from libopenage.main cimport main_arguments, run_game as run_game_cpp
from libopenage.error.handlers cimport set_exit_ok


def run_game(args, assets):
    """ Translates args and calls run_game_cpp. """
    # TODO port libopenage to use the fslike 'assets' object.

    del assets  # unused for now.

    cdef main_arguments args_cpp;
    cdef int result

    set_exit_ok(False)
    try:
        args_cpp.data_directory = args.asset_dir.encode()
        if args.fps is not None:
            args_cpp.fps_limit = args.fps
        else:
            args_cpp.fps_limit = 0

        args_cpp.gl_debug = args.gl_debug

        with nogil:
            result = run_game_cpp(args_cpp)

        return result
    finally:
        set_exit_ok(True)

# Copyright 2015-2015 the openage authors. See copying.md for legal info.


from libopenage.main cimport main_arguments, run_game as run_game_cpp


def run_game(args, assets):
    """ Translates args and calls run_game_cpp. """
    # TODO port libopenage to use the fslike 'assets' object.

    del assets  # unused for now.

    cdef main_arguments args_cpp;

    args_cpp.data_directory = args.asset_dir.encode()

    cdef int result

    with nogil:
        result = run_game_cpp(args_cpp)

    return result

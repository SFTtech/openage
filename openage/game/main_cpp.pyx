# Copyright 2015-2015 the openage authors. See copying.md for legal info.


from cpp.main cimport main_arguments, run_game as run_game_cpp


def run_game(args):
    """ Translates args and calls run_game_cpp. """

    cdef main_arguments args_cpp;

    args_cpp.data_directory = args.data_dir.encode()

    cdef int result

    with nogil:
        result = run_game_cpp(args_cpp)

    return result

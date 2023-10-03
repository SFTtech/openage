# Copyright 2015-2023 the openage authors. See copying.md for legal info.

from cpython.ref cimport PyObject
from libcpp.string cimport string
from libcpp.vector cimport vector

from libopenage.main cimport main_arguments, run_game as run_game_cpp
from libopenage.util.path cimport Path as Path_cpp
from libopenage.pyinterface.pyobject cimport PyObj
from libopenage.error.handlers cimport set_exit_ok
from libopenage.renderer.window cimport WindowConfig
from libopenage.renderer.enums cimport RenderMode

def run_game(args, root_path,window_args):
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

        # opengl debugging
        args_cpp.gl_debug = args.gl_debug

        # headless mode
        args_cpp.headless = args.headless

        # mods
        if args.modpacks is not None:
            args_cpp.mods = args.modpacks
        else:
            args_cpp.mods = vector[string]()

        cdef WindowConfig window_config
        window_config.width = window_args['width']
        window_config.height = window_args['height']
        window_config.vsync = window_args['vsync']
        
        # Determine the render mode based on window-related arguments
        if window_args['fullscreen']:
            window_config.render_mode = RenderMode.FULLSCREEN
        elif window_args['borderless']:
            window_config.render_mode = RenderMode.BORDERLESS
        else:
            window_config.render_mode = RenderMode.WINDOWED

        # Set the window configuration in args_cpp
        args_cpp.window_config = window_config
        # run the game!
        with nogil:
            result = run_game_cpp(args_cpp)

        return result
    finally:
        set_exit_ok(True)

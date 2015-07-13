# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Contains the function that initializes the C++ interface.
"""

from ..util.decorators import run_once

from ..log import dbg

from ..log.log_cpp import enable_log_translation


@run_once
def setup():
    """
    After a call to setup(), the C++ interface is in a usable state.

    setup() automatically checks whether there are any remaining un-initialized
    PyFunc objects, and raises an Exception if so.

    Must be invoked before any functions from libopenage are invoked.
    Runs only once; any subsequent invocations are a no-op, so don't hesitate
    to call this method whenever likely.

    Do _not_ invoke from a .pyx extension module that itself provides
    a setup method (circular imports)!
    """
    dbg("initializing libopenage...")

    # this is where calls to the setup methods of all other modules belong.
    from .exctranslate import setup as exctranslate_setup
    exctranslate_setup()

    from .exctranslate_tests import setup as exctranslate_tests_setup
    exctranslate_tests_setup()

    from .pyobject import setup as pyobject_setup
    pyobject_setup()

    from ..util.fslike_cpp import setup as fslike_cpp_setup
    fslike_cpp_setup()

    # verify that everything has been properly initialized.
    from .setup_checker import check
    check()

    enable_log_translation()

    dbg("C++ <-> Python interface has been initialized")

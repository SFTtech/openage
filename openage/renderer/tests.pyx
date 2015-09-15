# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
tests for the graphics renderer.
"""

import argparse

from libopenage.renderer.tests cimport renderer_demo as renderer_demo_c

def renderer_demo(list argv):
    """
    invokes the available render demos.
    """

    cmd = argparse.ArgumentParser(
        prog='... renderer_demo',
        description='Various renderer demos')
    cmd.add_argument("test_id", type=int, help="id of the demo to run.")

    args = cmd.parse_args(argv)

    cdef int renderer_test_id = args.test_id

    with nogil:
        renderer_demo_c(renderer_test_id)

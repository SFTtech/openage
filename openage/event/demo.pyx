# Copyright 2018-2018 the openage authors. See copying.md for legal info.

"""
demo for the event system.
"""

import argparse
from libcpp cimport bool

import openage.config as config
from openage.util.math import clamp
from openage.log import set_loglevel
from libopenage.event.demo.main cimport curvepong as curvepong_c


def curvepong(list argv):
    """
    starts curvepong, the event prediction demonstration with pong.
    """

    cmd = argparse.ArgumentParser(
        prog='... curvepong',
        description='Curvepong demonstration for the event prediction engine')

    if config.WITH_NCURSES:
        cmd.add_argument("--disable-gui", action='store_true',
                         help="Don't show the ncurses GUI")

    cmd.add_argument("--no-human", action='store_true',
                     help="Don't allow the human to play, use 2 AIs instead")
    cmd.add_argument("-v", "--verbose", action="count", default=0,
                     help="increase program verbosity")
    cmd.add_argument("-q", "--quiet", action="count", default=0,
                     help="decrease program verbosity")

    args = cmd.parse_args(argv)

    # default = 30 WARNING, the q and v args in/decrease it
    set_loglevel(clamp(30 - (args.verbose - args.quiet) * 10, 0, 50))

    cdef bool disable_gui = False
    if config.WITH_NCURSES:
        disable_gui = args.disable_gui

    cdef bool no_human = args.no_human

    with nogil:
        curvepong_c(disable_gui, no_human)

# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Convert a single slp file from some drs archive to a png image.
"""

from pathlib import Path

from .colortable import ColorTable
from .drs import DRS
from .texture import Texture
from ..util.fslike.directory import Directory
from ..log import info


def init_subparser(cli):
    """ Initializes the parser for convert-specific args. """
    import argparse

    cli.set_defaults(entrypoint=main)

    cli.add_argument("--palette", help=("palette number"
                                        "If not set, the SLP is assumed to "
                                        "support 32-Bit RGBA mode"))
    cli.add_argument("--interfac", type=argparse.FileType('rb'),
                     help=("drs archive where palette "
                           "is contained (interfac.drs). "
                           "If not set, assumed to be in same "
                           "directory as the source drs archive"))
    cli.add_argument("--drs", type=argparse.FileType('rb'),
                     help=("drs archive filename that contains an slp "
                           "e.g. path ~/games/aoe/graphics.drs"))
    cli.add_argument("slp", help=("slp filename "
                                  "e.g. path ~/games/aoe/326.slp"))
    cli.add_argument("output", help="image output path name")


def main(args, error):
    """ CLI entry point for single file conversions """
    del error  # unused

    slppath = Path(args.slp)
    outputpath = Path(args.output)

    # here, try opening slps from interfac or whereever
    if args.drs:
        drspath = Path(args.drs.name)

        # open from drs archive
        info("opening slp in drs '%s:%s'...", drspath, args.slp)
        slpfile = DRS(args.drs).root[args.slp].open("rb")

    else:
        # open directly from unpacked slp

        slpfile = slppath.open("rb")

    # open palette from interfac.drs file
    if args.palette:
        if args.interfac:
            interfacfile = args.interfac

        elif args.drs:
            # if no interfac was given, but a drs, assume
            # the same path of the drs archive.
            drspath = Path(args.drs.name)

            interfacfile = drspath.with_name("interfac.drs").open("rb")  # pylint: disable=no-member

        else:
            # otherwise use the path of the slp.

            interfacfile = slppath.with_name("interfac.drs").open("rb")  # pylint: disable=no-member

        # open palette
        info("opening palette in drs '%s:%s.bina'...", interfacfile.name, args.palette)
        palettefile = DRS(interfacfile).root["%s.bina" % args.palette].open("rb")

        info("parsing palette data...")
        palette = ColorTable(palettefile.read())


    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import SLP

    # parse the slp image
    info("parsing slp image...")
    slpimage = SLP(slpfile.read())

    # create texture
    info("packing texture...")
    tex = Texture(slpimage, args.palette)

    # to save as png:
    tex.save(Directory(outputpath.parent).root, outputpath.name)

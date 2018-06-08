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

    cli.add_argument("--palette", default="50500", help="palette number")
    cli.add_argument("--interfac", type=argparse.FileType('rb'),
                     help=("drs archive where palette "
                           "is contained (interfac.drs). "
                           "If not set, assumed to be in same "
                           "directory as the source drs archive"))
    cli.add_argument("drs", type=argparse.FileType('rb'),
                     help=("drs archive filename that contains the slp "
                           "e.g. path ~/games/aoe/graphics.drs"))
    cli.add_argument("slp", help=("slp filename inside the drs archive "
                                  "e.g. 326.slp"))
    cli.add_argument("output", help="image output path name")


def main(args, error):
    """ CLI entry point for single file conversions """
    del error  # unused

    drspath = Path(args.drs.name)
    outputpath = Path(args.output)

    if args.interfac:
        interfacfile = args.interfac
    else:
        # if no interfac was given, assume
        # the same path of the drs archive.

        interfacfile = drspath.with_name("interfac.drs").open("rb")  # pylint: disable=no-member

    # here, try opening slps from interfac or whereever
    info("opening slp in drs '%s:%s'...", drspath, args.slp)
    slpfile = DRS(args.drs).root[args.slp].open("rb")

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import SLP

    # parse the slp image
    info("parsing slp image...")
    slpimage = SLP(slpfile.read())

    # open color palette
    info("opening palette in drs '%s:%s.bina'...", interfacfile.name, args.palette)
    palettefile = DRS(interfacfile).root["%s.bina" % args.palette].open("rb")

    info("parsing palette data...")
    palette = ColorTable(palettefile.read())

    # create texture
    info("packing texture...")
    tex = Texture(slpimage, palette)

    # to save as png:
    tex.save(Directory(outputpath.parent).root, outputpath.name)

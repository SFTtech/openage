# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Convert a single slp file from some drs archive to a png image.
"""

import os.path

from .colortable import ColorTable
from .drs import DRS
from .texture import Texture
from ..util.fslike.directory import Directory
from ..log import info


def init_subparser(cli):
    """ Initializes the parser for convert-specific args. """
    cli.set_defaults(entrypoint=main)

    cli.add_argument("--palette", default="50500", help="palette number")
    cli.add_argument("--interfac", help=("drs archive where palette "
                                         "is contained (interfac.drs)"))
    cli.add_argument("drs", help=("drs archive filename that contains the slp "
                                  "e.g. path ~/games/aoe/graphics.drs"))
    cli.add_argument("slp", help=("slp filename inside the drs archive "
                                  "e.g. 326.slp"))
    cli.add_argument("output", help="image output path name")


def main(args, error):
    """ CLI entry point for single file conversions """
    del error  # unused

    if args.interfac:
        interfacfile = args.interfac
    else:
        # if no interfac was given, assume
        # the same path of the drs archive.
        drspath = os.path.split(args.drs)[0]
        interfacfile = os.path.join(drspath, "interfac.drs")

    # if .png was passed, strip it away.
    if args.output.endswith(".png"):
        args.output = args.output[:-4]

    # here, try opening slps from interfac or whereever
    info("opening slp in drs '%s:%s'..." % (args.drs, args.slp))
    slpfile = DRS(open(args.drs, "rb")).root[args.slp].open("rb")

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import SLP

    # parse the slp image
    info("parsing slp image...")
    slpimage = SLP(slpfile.read())

    # open color palette
    info("opening palette in drs '%s:%s.bina'..." % (interfacfile,
                                                     args.palette))
    palettefile = DRS(open(interfacfile, "rb")).\
        root["%s.bina" % args.palette].open("rb")
    info("parsing palette data...")
    palette = ColorTable(palettefile.read())

    # create texture
    info("packing texture...")
    tex = Texture(slpimage, palette)

    # to save as png:
    path, filename = os.path.split(args.output)
    tex.save(Directory(path).root, filename)

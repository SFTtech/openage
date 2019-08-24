# Copyright 2015-2019 the openage authors. See copying.md for legal info.

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

    cli.add_argument("--palette-index", default="50500",
                     help="palette number in interfac.drs")
    cli.add_argument("--palette-file", type=argparse.FileType('rb'),
                     help=("palette file where the palette"
                           "colors are contained"))
    cli.add_argument("--player-palette-file", type=argparse.FileType('rb'),
                     help=("palette file where the player"
                           "colors are contained"))
    cli.add_argument("--interfac", type=argparse.FileType('rb'),
                     help=("drs archive where palette "
                           "is contained (interfac.drs). "
                           "If not set, assumed to be in same "
                           "directory as the source drs archive"))
    cli.add_argument("--drs", type=argparse.FileType('rb'),
                     help=("drs archive filename that contains an slp "
                           "e.g. path ~/games/aoe/graphics.drs"))
    cli.add_argument("--mode",
                     help=("choose between DRS-SLP, SLP or SMP; "
                           "otherwise, this is determined by the file extension"))
    cli.add_argument("filename", help=("filename "
                                       "e.g. path ~/games/aoe/326.slp"))
    cli.add_argument("output", help="image output path name")


def main(args, error):
    """ CLI entry point for single file conversions """
    del error  # unused

    file_path = Path(args.filename)
    file_extension = file_path.suffix[1:].upper()

    if args.mode == "SLP" or (file_extension == "SLP" and not args.drs):
        if not args.palette_file:
            raise Exception("palette-file needs to be specified")
        
        SLP_file(args.filename, args.palette_file, args.output)
 
    elif args.mode == "DRS-SLP" or (file_extension == "SLP" and args.drs):
        if not (args.drs and args.palette_index):
            raise Exception("palette-file needs to be specified")
        
        SLP_in_DRS_file(args.drs, args.filename, args.palette_index,
                        args.output, interfac=args.interfac)
     
    elif args.mode == "SMP" or file_extension == "SMP":
        if not (args.palette_file and args.player_palette_file):
            raise Exception("palette-file needs to be specified")
        
        SMP_file(args.filename, args.palette_file, args.player_palette_file,
                 args.output)
     
    else:
        raise Exception("format could not be determined")


def SLP_file(slp_path, palette, output_path):
    """
    Reads a single SLP file.
    """
    output_file = Path(output_path)

    # open the slp
    info("opening slp file at '%s'", Path(slp_path).name)
    slp_file = Path(slp_path).open("rb")

    # open palette from independent file
    info("opening palette in palette file '%s'", palette.name)
    palette_file = Path(palette.name).open("rb")

    info("parsing palette data...")
    palette = ColorTable(palette_file.read())

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import SLP

    # parse the slp_path image
    info("parsing slp image...")
    slp_image = SLP(slp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, palette)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def SLP_in_DRS_file(drs, slp_path, palette_index, output_path, interfac=None):
    """
    Reads a SLP file from a DRS archive.
    """
    output_file = Path(output_path)
    
    # open from drs archive
    drs_file = DRS(drs)
    
    info("opening slp in drs '%s:%s'...", drs.name, slp_path)
    slp_file = drs_file.root[slp_path].open("rb")
    
    if interfac:
        # open the interface file if given 
        interfac_file = interfac
    
    else:
        # otherwise use the path of the drs.
        interfac_file = Path(drs.name).with_name("interfac.drs").open("rb")  # pylint: disable=no-member
        
    # open palette
    info("opening palette in drs '%s:%s.bina'...", interfac_file.name, palette_index)
    palette_file = DRS(interfac_file).root["%s.bina" % palette_index].open("rb")

    info("parsing palette data...")
    palette = ColorTable(palette_file.read())

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import SLP

    # parse the slp image
    info("parsing slp image...")
    slp_image = SLP(slp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, palette)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def SMP_file(smp_path, main_palette, player_palette, output_path):
    """
    Reads a single SMP file.
    """
    output_file = Path(output_path)
    
    # open the smp
    info("opening smp file at '%s'", smp_path)
    smp_file = Path(smp_path).open("rb")

    # open main palette from independent file
    info("opening main palette in palette file '%s'", main_palette.name)
    main_palette_file = Path(main_palette.name).open("rb")

    info("parsing palette data...")
    main_palette_table = ColorTable(main_palette_file.read())

    # open player color palette from independent file
    info("opening player color palette in palette file '%s'", player_palette.name)
    player_palette_file = Path(player_palette.name).open("rb")

    info("parsing palette data...")
    player_palette_table = ColorTable(player_palette_file.read())
    
    # import here to prevent that the __main__ depends on SMP
    # just by importing this singlefile.py.
    from .smp import SMP

    # parse the slp_path image
    info("parsing smp image...")
    smp_image = SMP(smp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smp_image, main_palette_table, player_palette_table)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)
    
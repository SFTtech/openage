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
    cli.add_argument("--mode", choices=['drs-slp', 'slp', 'smp', 'smx'],
                     help=("choose between drs-slp, slp, smp or smx; "
                           "otherwise, this is determined by the file extension"))
    cli.add_argument("filename", help=("filename or, if inside a drs archive "
                                       "given by --drs, the filename within "
                                       "the drs archive"))
    cli.add_argument("output", help="image output path name")


def main(args, error):
    """ CLI entry point for single file conversions """
    del error  # unused

    file_path = Path(args.filename)
    file_extension = file_path.suffix[1:].lower()

    if args.mode == "slp" or (file_extension == "slp" and not args.drs):
        if not args.palette_file:
            raise Exception("palette-file needs to be specified")

        read_slp_file(args.filename, args.palette_file, args.output,
                      player_palette=args.player_palette_file)

    elif args.mode == "drs-slp" or (file_extension == "slp" and args.drs):
        if not (args.drs and args.palette_index):
            raise Exception("palette-file needs to be specified")
        
        # Only for the test of this draft.
        #read_slp_in_drs_file(args.drs, args.filename, args.palette_index,
                             #args.output, interfac=args.interfac)

        terrain_convert(args.drs, args.filename, args.palette_index, args.output, interfac = args.interfac)

    elif args.mode == "smp" or file_extension == "smp":
        if not (args.palette_file and args.player_palette_file):
            raise Exception("palette-file needs to be specified")

        read_smp_file(args.filename, args.palette_file, args.player_palette_file,
                      args.output)

    elif args.mode == "smx" or file_extension == "smx":
        if not (args.palette_file and args.player_palette_file):
            raise Exception("palette-file needs to be specified")

        read_smx_file(args.filename, args.palette_file, args.player_palette_file,
                      args.output)

    else:
        raise Exception("format could not be determined")


def read_slp_file(slp_path, main_palette, output_path, player_palette=None):
    """
    Reads a single SLP file.
    """
    output_file = Path(output_path)

    # open the slp
    info("opening slp file at '%s'", Path(slp_path).name)
    slp_file = Path(slp_path).open("rb")

    # open palette from independent file
    info("opening palette in palette file '%s'", main_palette.name)
    palette_file = Path(main_palette.name).open("rb")

    info("parsing palette data...")
    main_palette_table = ColorTable(palette_file.read())

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import SLP

    # parse the slp_path image
    info("parsing slp image...")
    slp_image = SLP(slp_file.read())

    player_palette_table = None

    # Player palettes need to be specified if SLP version is greater
    # than 3.0
    if slp_image.version in (b'3.0\x00', b'4.0X', b'4.1X'):
        if not player_palette:
            raise Exception("SLPs version %s require a player "
                            "color palette" % slp_image.version)

        # open player color palette from independent file
        info("opening player color palette in palette file '%s'", player_palette.name)
        player_palette_file = Path(player_palette.name).open("rb")

        info("parsing palette data...")
        player_palette_table = ColorTable(player_palette_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, main_palette_table, player_palette_table)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def read_slp_in_drs_file(drs, slp_path, palette_index, output_path, interfac=None):
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
        interfac_file = Path(drs.name).with_name(
            "interfac.drs").open("rb")  # pylint: disable=no-member

    # open palette
    info("opening palette in drs '%s:%s.bina'...",
         interfac_file.name, palette_index)
    palette_file = DRS(
        interfac_file).root["%s.bina" % palette_index].open("rb")

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


def read_smp_file(smp_path, main_palette, player_palette, output_path):
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

    # parse the smp_path image
    info("parsing smp image...")
    smp_image = SMP(smp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smp_image, main_palette_table, player_palette_table)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def read_smx_file(smx_path, main_palette, player_palette, output_path):
    """
    Reads a single SMX (compressed SMP) file.
    """
    output_file = Path(output_path)

    # open the smx
    info("opening smx file at '%s'", smx_path)
    smx_file = Path(smx_path).open("rb")

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
    from .smx import SMX

    # parse the smx_path image
    info("parsing smx image...")
    smx_image = SMX(smx_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smx_image, main_palette_table, player_palette_table)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def terrain_convert(drs, slp_path, palette_index, output_path, interfac=None):
    """
    Reads a SLP from a DRS archive and output a texture from it. It uses Cython code (for now pure Python), which is faster.
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
        interfac_file = Path(drs.name).with_name(
            "interfac.drs").open("rb")  # pylint: disable=no-member

    # open palette
    info("opening palette in drs '%s:%s.bina'...",
         interfac_file.name, palette_index)
    palette_file = DRS(
        interfac_file).root["%s.bina" % palette_index].open("rb")

    info("parsing palette data...")
    palette = ColorTable(palette_file.read())

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from .slp import determine_rgba_matrix, SLP, SLPFrame, FrameInfo
    from PIL import Image
    from .texture import Texture, TextureImage
    from .terrain_convert import Terrain_convert
    import numpy as np
    

    # parse the slp image
    info("parsing slp image...")
    slp_image = SLP(slp_file.read())
    
    # In order to extract the special image data from the SLP, I have to merge first all the frames ?
    # In this case, the format is bad, we have a ndarray(10,10,dtype=np.ndarray(49,97),ndtype=tuple4), where each ndarray is of
    # size 49x97x4
    # How do we need to pass "special SLP image data" to PIL Image ?
    # Should we have a matrix of 490x970 with each pixel is a rgba tuple?
    info("parsing SLP Image into RGBA Matrix")
    image = np.empty([10,10],dtype=np.ndarray)
    row=0
    column=0
    count=0
    while count < 100:
        if row % 10 ==0 and row > 0:
            column+=1
            row=0
        image[row][column] = slp_image.main_frames[count].get_picture_data(palette)
        row+=1
        count+=1

    #matrix = determine_rgba_matrix(image, None, 0) ???
    image_new = Image.fromarray(image)
    info("merging RGBA Matrix")
    merged_image = Terrain_convert.merge(image_new)

    info("transforming RGBA Matrix")
    transformed_image = Terrain_convert.transform(merged_image)
    
    
    #transform RGBA matrix into a Texture or TextureImage ?
    info("rgba->TextureImage")
    final_texture = TextureImage(transformed_image)


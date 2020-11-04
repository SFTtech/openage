# Copyright 2015-2020 the openage authors. See copying.md for legal info.

"""
Convert a single slp/wav file from some drs archive to a png/opus file.
"""

from pathlib import Path

from ...log import info
from ...util.fslike.directory import Directory
from ..entity_object.export.texture import Texture
from ..value_object.read.media.colortable import ColorTable
from ..value_object.read.media.drs import DRS


def init_subparser(cli):
    """ Initializes the parser for convert-specific args. """
    import argparse

    cli.set_defaults(entrypoint=main)

    cli.add_argument("--palettes-path",
                     help=("path to the folder containing the palettes.conf file "
                           "OR an interfac.drs archive that contains palette files"))
    cli.add_argument("--drs", type=argparse.FileType('rb'),
                     help=("drs archive filename that contains an slp or wav "
                           "e.g. path ~/games/aoe/graphics.drs"))
    cli.add_argument("--mode", choices=['drs-slp', 'drs-wav', 'slp', 'smp', 'smx', 'wav'],
                     help=("choose between drs-slp, drs-wav, slp, smp, smx or wav; "
                           "otherwise, this is determined by the file extension"))
    cli.add_argument("filename", help=("filename or, if inside a drs archive "
                                       "given by --drs, the filename within "
                                       "the drs archive"))
    cli.add_argument("output", help="output path name")


def main(args, error):
    """ CLI entry point for single file conversions """
    del error  # unused

    file_path = Path(args.filename)
    file_extension = file_path.suffix[1:].lower()

    if not (args.mode in ["drs-wav", "wav"] or file_extension == "wav"):
        if not args.palettes_path:
            raise Exception("palettes-path needs to be specified")

        palettes_path = Path(args.palettes_path)
        palettes = read_palettes(palettes_path)

    if args.mode == "slp" or (file_extension == "slp" and not args.drs):
        read_slp_file(args.filename, args.output, palettes)

    elif args.mode == "drs-slp" or (file_extension == "slp" and args.drs):
        read_slp_in_drs_file(args.drs, args.filename, args.output, palettes)

    elif args.mode == "smp" or file_extension == "smp":
        read_smp_file(args.filename, args.output, palettes)

    elif args.mode == "smx" or file_extension == "smx":
        read_smx_file(args.filename, args.output, palettes)

    elif args.mode == "wav" or (file_extension == "wav" and not args.drs):
        read_wav_file(args.filename, args.output)

    elif args.mode == "drs-wav" or (file_extension == "wav" and args.drs):
        read_wav_in_drs_file(args.drs, args.filename, args.output)

    else:
        raise Exception("format could not be determined")


def read_palettes(palettes_path):
    """
    Reads the palettes from the palettes folder/archive.
    """
    palettes = {}

    if palettes_path.is_dir():
        info("opening palette files in directory '%s'", palettes_path.name)

        palette_dir = Directory(palettes_path)
        conf_filepath = "palettes.conf"
        conf_file = palette_dir.root[conf_filepath].open('rb')
        palette_paths = {}

        info("parsing palette data...")
        for line in conf_file.read().decode('utf-8').split('\n'):
            line = line.strip()

            # skip comments and empty lines
            if not line or line.startswith('//'):
                continue

            palette_id, filepath = line.split(',')
            palette_id = int(palette_id)
            palette_paths[palette_id] = filepath

        for palette_id, filepath in palette_paths.items():
            palette_file = palette_dir.root[filepath]
            palette = ColorTable(palette_file.open("rb").read())

            palettes[palette_id] = palette

    else:
        info("opening palette files in drs archive '%s'", palettes_path.name)

        # open from drs archive
        # TODO: Also allow SWGB's DRS files
        palette_file = Path(palettes_path).open("rb")
        game_version = ("AOC", [])
        palette_dir = DRS(palette_file, game_version)

        info("parsing palette data...")
        for palette_file in palette_dir.root.iterdir():
            # Only 505XX.bina files are usable palettes
            if palette_file.stem.startswith("505"):
                palette = ColorTable(palette_file.open("rb").read())
                palette_id = int(palette_file.stem)

                palettes[palette_id] = palette

    return palettes


def read_slp_file(slp_path, output_path, palettes):
    """
    Reads a single SLP file.
    """
    output_file = Path(output_path)

    # open the slp
    info("opening slp file at '%s'", Path(slp_path).name)
    slp_file = Path(slp_path).open("rb")

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from ..value_object.read.media.slp import SLP

    # parse the slp_path image
    info("parsing slp image...")
    slp_image = SLP(slp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, palettes)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def read_slp_in_drs_file(drs, slp_path, output_path, palettes):
    """
    Reads a SLP file from a DRS archive.
    """
    output_file = Path(output_path)

    # open from drs archive
    # TODO: Also allow SWGB's DRS files
    game_version = ("AOC", [])
    drs_file = DRS(drs, game_version)

    info("opening slp in drs '%s:%s'...", drs.name, slp_path)
    slp_file = drs_file.root[slp_path].open("rb")

    # import here to prevent that the __main__ depends on SLP
    # just by importing this singlefile.py.
    from ..value_object.read.media.slp import SLP

    # parse the slp image
    info("parsing slp image...")
    slp_image = SLP(slp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, palettes)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def read_smp_file(smp_path, output_path, palettes):
    """
    Reads a single SMP file.
    """
    output_file = Path(output_path)

    # open the smp
    info("opening smp file at '%s'", smp_path)
    smp_file = Path(smp_path).open("rb")

    # import here to prevent that the __main__ depends on SMP
    # just by importing this singlefile.py.
    from ..value_object.read.media.smp import SMP

    # parse the smp_path image
    info("parsing smp image...")
    smp_image = SMP(smp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smp_image, palettes)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def read_smx_file(smx_path, output_path, palettes):
    """
    Reads a single SMX (compressed SMP) file.
    """
    output_file = Path(output_path)

    # open the smx
    info("opening smx file at '%s'", smx_path)
    smx_file = Path(smx_path).open("rb")

    # import here to prevent that the __main__ depends on SMP
    # just by importing this singlefile.py.
    from ..value_object.read.media.smx import SMX

    # parse the smx_path image
    info("parsing smx image...")
    smx_image = SMX(smx_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smx_image, palettes)

    # save as png
    tex.save(Directory(output_file.parent).root, output_file.name)


def read_wav_file(wav_path, output_path):
    """
    Reads a single WAV file.
    """

    output_file = Path(output_path)

    # open the wav file
    info("opening wav file at '%s'", wav_path)
    wav_file = Path(wav_path).open("rb")

    # import here to prevent that the __main__ depends on opusenc
    # just by importing this singlefile.py.
    from ..service.export.opus.opusenc import encode

    # convert wav to opus
    info("converting wav to opus...")
    opus_data = encode(wav_file.read())

    # save converted opus data to target directory
    info("saving opus file...")
    output_file.write_bytes(opus_data)


def read_wav_in_drs_file(drs, wav_path, output_path):
    """
    Reads a WAV file from a DRS archive.
    """
    output_file = Path(output_path)

    # open from drs archive
    # TODO: Also allow SWGB's DRS files
    game_version = ("AOC", [])
    drs_file = DRS(drs, game_version)

    info("opening wav in drs '%s:%s'...", drs.name, wav_path)
    wav_file = drs_file.root[wav_path].open("rb")

    # import here to prevent that the __main__ depends on opusenc
    # just by importing this singlefile.py.
    from ..service.export.opus.opusenc import encode

    # convert wav to opus
    info("converting wav to opus...")
    opus_data = encode(wav_file.read())

    # save converted opus data to target directory
    info("saving opus file...")
    output_file.write_bytes(opus_data)

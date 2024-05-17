# Copyright 2015-2024 the openage authors. See copying.md for legal info.

"""
Convert a single slp/wav file from some drs archive to a png/opus file.
"""
from __future__ import annotations

import sys

from pathlib import Path

from openage.convert.processor.export.media_exporter import MediaExporter
from openage.convert.value_object.init.game_version import GameEdition, GameVersion

from ...log import info
from ...util.fslike.directory import Directory
from ..entity_object.export.texture import Texture
from ..value_object.read.media.colortable import ColorTable
from ..value_object.read.media.drs import DRS

AOC_GAME_VERSION = GameVersion(
    edition=GameEdition("Dummy AOC", "AOC", "YES", [], [], {}, [], [])
)
SWGB_GAME_VERSION = GameVersion(
    edition=GameEdition("Dummy SWGB", "SWGB", "YES", [], [], {}, [], [])
)


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
    cli.add_argument("--mode", choices=['drs-slp', 'drs-wav', 'sld', 'slp', 'smp', 'smx', 'wav'],
                     help=("choose between drs-slp, drs-wav, sld, slp, smp, smx or wav; "
                           "otherwise, this is determined by the file extension"))
    cli.add_argument("--compression-level", type=int, default=2, choices=[0, 1, 2, 3, 4],
                     help="set PNG compression level")
    cli.add_argument("--layer", type=int, default=0, choices=[0, 1, 2, 3, 4],
                     help="ID of SLD/SMP/SMX layer that should be exported to image file")
    cli.add_argument("filename", help=("filename or, if inside a drs archive "
                                       "given by --drs, the filename within "
                                       "the drs archive"))
    cli.add_argument("output", help="output path")


def main(args, error):
    """
    CLI entry point for single file conversions
    """
    del error  # unused

    file_path = Path(args.filename)
    file_extension = file_path.suffix[1:].lower()

    if sys.platform == "win32":
        from openage.util.dll import DllDirectoryManager, default_paths
        dll_manager = DllDirectoryManager(default_paths())
        dll_manager.add_directories()

    if not (args.mode in ("sld", "drs-wav", "wav") or file_extension in ("sld", "wav")):
        if not args.palettes_path:
            raise RuntimeError("palettes-path needs to be specified for "
                               f"file type '{file_extension}'")

        palettes_path = Path(args.palettes_path)
        palettes = read_palettes(palettes_path)

    compression_level = args.compression_level
    layer = args.layer
    if args.mode == "slp" or (file_extension == "slp" and not args.drs):
        read_slp_file(args.filename, args.output, palettes, compression_level)

    elif args.mode == "drs-slp" or (file_extension == "slp" and args.drs):
        read_slp_in_drs_file(args.drs, args.filename, args.output, palettes, compression_level)

    elif args.mode == "smp" or file_extension == "smp":
        read_smp_file(args.filename, args.output, palettes, compression_level, layer)

    elif args.mode == "smx" or file_extension == "smx":
        read_smx_file(args.filename, args.output, palettes, compression_level, layer)

    elif args.mode == "sld" or file_extension == "sld":
        read_sld_file(args.filename, args.output, compression_level, layer)

    elif args.mode == "wav" or (file_extension == "wav" and not args.drs):
        read_wav_file(args.filename, args.output)

    elif args.mode == "drs-wav" or (file_extension == "wav" and args.drs):
        read_wav_in_drs_file(args.drs, args.filename, args.output)

    else:
        raise SyntaxError("format could not be determined")


def read_palettes(palettes_path: Path) -> dict[str, ColorTable]:
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
        with Path(palettes_path).open("rb") as palette_file:
            game_version = AOC_GAME_VERSION
            palette_dir = DRS(palette_file, game_version)

            info("parsing palette data...")
            for palette_file in palette_dir.root.iterdir():
                # Only 505XX.bina files are usable palettes
                if palette_file.stem.startswith("505"):
                    palette = ColorTable(palette_file.open("rb").read())
                    palette_id = int(palette_file.stem)

                    palettes[palette_id] = palette

    return palettes


def read_slp_file(
    slp_path: Path,
    output_path: Path,
    palettes: dict[str, ColorTable],
    compression_level: int
) -> None:
    """
    Reads a single SLP file.
    """
    output_file = Path(output_path)

    # open the slp
    info("opening slp file at '%s'", Path(slp_path).name)
    with Path(slp_path).open("rb") as slp_file:
        # import here to prevent that the __main__ depends on SLP
        # just by importing this singlefile.py.
        from ..value_object.read.media.slp import SLP

        # parse the slp_path image
        info("parsing slp image...")
        slp_image = SLP(slp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, palettes)

    from ..processor.export.texture_merge import merge_frames
    try:
        merge_frames(tex)

    except ValueError:
        info("slp contains 0 frames! aborting texture export.")
        return

    # save as png
    info("saving png file at '%s'", output_path)
    MediaExporter.save_png(
        tex,
        Directory(output_file.parent).root,
        output_file.name,
        compression_level
    )


def read_slp_in_drs_file(
    drs: Path,
    slp_path: Path,
    output_path: Path,
    palettes: dict[str, ColorTable],
    compression_level: int
) -> None:
    """
    Reads a SLP file from a DRS archive.
    """
    output_file = Path(output_path)

    # open from drs archive
    game_version = AOC_GAME_VERSION
    drs_file = DRS(drs, game_version)

    info("opening slp in drs '%s:%s'...", drs.name, slp_path)
    with drs_file.root[slp_path].open("rb") as slp_file:
        # import here to prevent that the __main__ depends on SLP
        # just by importing this singlefile.py.
        from ..value_object.read.media.slp import SLP

        # parse the slp image
        info("parsing slp image...")
        slp_image = SLP(slp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(slp_image, palettes)

    from ..processor.export.texture_merge import merge_frames
    try:
        merge_frames(tex)

    except ValueError:
        info("slp contains 0 frames! aborting texture export.")
        return

    # save as png
    info("saving png file at '%s'", output_path)
    MediaExporter.save_png(
        tex,
        Directory(output_file.parent).root,
        output_file.name,
        compression_level
    )


def read_smp_file(
    smp_path: Path,
    output_path: Path,
    palettes: dict[str, ColorTable],
    compression_level: int,
    layer: int
) -> None:
    """
    Reads a single SMP file.
    """
    output_file = Path(output_path)

    # open the smp
    info("opening smp file at '%s'", smp_path)
    with Path(smp_path).open("rb") as smp_file:
        # import here to prevent that the __main__ depends on SMP
        # just by importing this singlefile.py.
        from ..value_object.read.media.smp import SMP

        # parse the smp_path image
        info("parsing smp image...")
        smp_image = SMP(smp_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smp_image, palettes)

    from ..processor.export.texture_merge import merge_frames
    try:
        merge_frames(tex)

    except ValueError:
        info("layer %s contains 0 frames! aborting texture export.", layer)
        return

    # save as png
    info("saving png file at '%s'", output_path)
    MediaExporter.save_png(
        tex,
        Directory(output_file.parent).root,
        output_file.name,
        compression_level
    )


def read_smx_file(
    smx_path: Path,
    output_path: Path,
    palettes: dict[str, ColorTable],
    compression_level: int,
    layer: int
) -> None:
    """
    Reads a single SMX (compressed SMP) file.
    """
    output_file = Path(output_path)

    # open the smx
    info("opening smx file at '%s'", smx_path)
    with Path(smx_path).open("rb") as smx_file:
        # import here to prevent that the __main__ depends on SMP
        # just by importing this singlefile.py.
        from ..value_object.read.media.smx import SMX

        # parse the smx_path image
        info("parsing smx image...")
        smx_image = SMX(smx_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(smx_image, palettes, layer=layer)

    from ..processor.export.texture_merge import merge_frames
    try:
        merge_frames(tex)

    except ValueError:
        info("layer %s contains 0 frames! aborting texture export.", layer)
        return

    # save as png
    info("saving png file at '%s'", output_path)
    MediaExporter.save_png(
        tex,
        Directory(output_file.parent).root,
        output_file.name,
        compression_level
    )


def read_sld_file(
    sld_path: Path,
    output_path: Path,
    compression_level: int,
    layer: int
) -> None:
    """
    Reads a single SMX (compressed SMP) file.
    """
    output_file = Path(output_path)

    # open the sld
    info("opening sld file at '%s'", sld_path)
    with Path(sld_path).open("rb") as smx_file:
        # import here to prevent that the __main__ depends on SMP
        # just by importing this singlefile.py.
        from ..value_object.read.media.sld import SLD

        # parse the smx_path image
        info("parsing sld image...")
        sld_image = SLD(smx_file.read())

    # create texture
    info("packing texture...")
    tex = Texture(sld_image, layer=layer)

    from ..processor.export.texture_merge import merge_frames
    try:
        merge_frames(tex)

    except ValueError:
        info("layer %s contains 0 frames! aborting texture export.", layer)
        return

    # save as png
    info("saving png file at '%s'", output_path)
    MediaExporter.save_png(
        tex,
        Directory(output_file.parent).root,
        output_file.name,
        compression_level
    )


def read_wav_file(wav_path: Path, output_path: Path) -> None:
    """
    Reads a single WAV file.
    """

    output_file = Path(output_path)

    # open the wav file
    info("opening wav file at '%s'", wav_path)
    with Path(wav_path).open("rb") as wav_file:
        # import here to prevent that the __main__ depends on opusenc
        # just by importing this singlefile.py.
        from ..service.export.opus.opusenc import encode

        # convert wav to opus
        info("converting wav to opus...")
        opus_data = encode(wav_file.read())

    # save converted opus data to target directory
    info("saving opus file...")
    output_file.write_bytes(opus_data)


def read_wav_in_drs_file(drs: Path, wav_path: Path, output_path: Path) -> None:
    """
    Reads a WAV file from a DRS archive.
    """
    output_file = Path(output_path)

    # open from drs archive
    game_version = AOC_GAME_VERSION
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

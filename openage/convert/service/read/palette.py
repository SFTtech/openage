# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Module for reading palette files.
"""
from ...value_object.dataformat.game_version import GameEdition
from ...value_object.dataformat.media_types import MediaType
from ...value_object.media.colortable import ColorTable


def get_palettes(srcdir, game_version, index=None):
    """
    Read and create the color palettes.
    """
    game_edition = game_version[0]

    palettes = {}

    if game_edition in (GameEdition.ROR, GameEdition.AOC, GameEdition.SWGB, GameEdition.HDEDITION):
        if index:
            palette_path = "%s/%s.bina" % (MediaType.PALETTES.value, str(index))
            palette_file = srcdir[palette_path]
            palette = ColorTable(palette_file.open("rb").read())
            palette_id = int(palette_file.stem)

            palettes[palette_id] = palette

        else:
            palette_dir = srcdir[MediaType.PALETTES.value]
            for palette_file in palette_dir.iterdir():
                # Only 505XX.bina files are usable palettes
                if palette_file.stem.startswith("505"):
                    palette = ColorTable(palette_file.open("rb").read())
                    palette_id = int(palette_file.stem)

                    palettes[palette_id] = palette

            if game_edition is GameEdition.HDEDITION:
                # TODO: HD edition has extra palettes in the dat folder
                pass

    elif game_edition in (GameEdition.AOE1DE, GameEdition.AOE2DE):
        # Parse palettes.conf file and save the ids/paths
        conf_filepath = "%s/palettes.conf" % (MediaType.PALETTES.value)
        conf_file = srcdir[conf_filepath].open('rb')
        palette_paths = {}

        for line in conf_file.read().decode('utf-8').split('\n'):
            line = line.strip()

            # skip comments and empty lines
            if not line or line.startswith('//'):
                continue

            palette_id, filepath = line.split(',')
            palette_id = int(palette_id)
            palette_paths[palette_id] = filepath

        if index:
            palette_path = "%s/%s" % (MediaType.PALETTES.value, palette_paths[index])
            palette = ColorTable(srcdir[palette_path].open("rb").read())

            palettes[index] = palette

        else:
            for palette_id, filepath in palette_paths.items():
                palette_path = "%s/%s" % (MediaType.PALETTES.value, filepath)
                palette_file = srcdir[palette_path]
                palette = ColorTable(palette_file.open("rb").read())

                palettes[palette_id] = palette

    return palettes

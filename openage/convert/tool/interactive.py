# Copyright 2020-2021 the openage authors. See copying.md for legal info.

"""
Interactive browser for game asset files.
"""

import os
import readline  # pylint: disable=unused-import

from openage.convert.processor.export.media_exporter import MediaExporter

from ...log import warn, info
from ...util.fslike.directory import Directory
from ..service.init.mount_asset_dirs import mount_asset_dirs
from ..service.init.version_detect import create_version_objects
from .subtool.version_select import get_game_version


def interactive_browser(cfg, srcdir, game_version):
    """
    launch an interactive view for browsing the original
    archives.

    TODO: Enhance functionality and fix SLP conversion.
    """

    info("launching interactive data browser...")

    # the variables are actually used, in the interactive prompt.
    # pylint: disable=possibly-unused-variable

    if not game_version[0]:
        warn("cannot launch browser as no valid game version was found.")
        return

    data = mount_asset_dirs(srcdir, game_version)

    if not data:
        warn("cannot launch browser as no valid input assets were found.")
        return

    def save(path, target):
        """
        save a path to a custom target
        """
        with path.open("rb") as infile:
            with open(target, "rb") as outfile:
                outfile.write(infile.read())

    def save_slp(path, target, palette=None):
        """
        save a slp as png.
        """
        from ..entity_object.export.texture import Texture
        from ..value_object.read.media.slp import SLP
        from ..service.read.palette import get_palettes

        if not palette:
            palette = get_palettes(data, game_version)

        with path.open("rb") as slpfile:

            from ..processor.export.texture_merge import merge_frames

            tex = Texture(SLP(slpfile.read()), palette)

            merge_frames(tex)

            out_path, filename = os.path.split(target)
            MediaExporter.save_png(
                tex,
                Directory(out_path).root,
                filename
            )

    def save_smx(path, target, palette=None):
        """
        save a smx as png.
        """
        from ..entity_object.export.texture import Texture
        from ..value_object.read.media.smx import SMX
        from ..service.read.palette import get_palettes

        if not palette:
            palette = get_palettes(data, game_version)

        with path.open("rb") as smxfile:

            from ..processor.export.texture_merge import merge_frames

            tex = Texture(SMX(smxfile.read()), palette)

            merge_frames(tex)

            out_path, filename = os.path.split(target)
            MediaExporter.save_png(
                tex,
                Directory(out_path).root,
                filename
            )

    import code
    from pprint import pprint

    import rlcompleter

    completer = rlcompleter.Completer(locals())
    readline.parse_and_bind("tab: complete")
    readline.parse_and_bind("set show-all-if-ambiguous on")
    readline.set_completer(completer.complete)

    code.interact(
        banner=("\nuse `pprint` for beautiful output!\n"
                "you can access stuff by the `data` variable!\n"
                "`data` is an openage.util.fslike.path.Path!\n\n"
                "* version detection:   pprint(game_versions)\n"
                "* list contents:       pprint(list(data['graphics'].list()))\n"
                "* dump data:           save(data['file/path'], '/tmp/outputfile')\n"
                "* save a slp as png:   save_slp(data['dir/123.slp'], '/tmp/pic.png')\n"
                "* save a smx as png:   save_smx(data['dir/123.smx'], '/tmp/pic.png')\n"),
        local=locals()
    )

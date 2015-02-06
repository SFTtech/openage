# Copyright 2015-2015 the openage authors. See copying.md for legal info.

from .drs import DRS


class MediaExtractor:
    def __init__(self, vd, args):
        self.vd = vd
        self.args = args

        self.metadata_formats = ("csv",)

        self.ensure_version_sanity()

    def ensure_version_sanity(self):
        if len(self.vd.get_files("interfac")) != 1:
            raise Exception("Exactly one interfac file needed, check version definitions.")

        if len(self.vd.get_files("blendomatic")) != 1:
            raise Exception("Exactly one blendomatic file needed, check version definitions.")

    def read_palettes(self):
        from .colortable import ColorTable, PlayerColorTable

        # 50500 is the file id of the color id lookup table
        interfac_drs = DRS(self.vd.get_files("interfac").pop())
        palette = ColorTable(interfac_drs.get_file_data('bin', 50500))
        player_palette = PlayerColorTable(palette)

        if self.args.extrafiles:
            palette.save_visualization('info/colortable.pal.png')
            player_palette.save_visualization('info/playercolortable.pal.png')

        # TODO: player_palette -> metadata dump

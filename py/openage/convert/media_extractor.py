# Copyright 2015-2015 the openage authors. See copying.md for legal info.

from .drs import DRS
import os


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

    def read_termcolortable(self):
        from .hardcoded.termcolors import urxvtcoltable
        from .colortable import ColorTable

        termcolortable = ColorTable(urxvtcoltable)

        # TODO: termcolortable -> metadata dump

    def read_blendomatic(self):
        from .blendomatic import Blendomatic

        blend_file = vd.get_files("blendomatic").pop()
        blend_data = Blendomatic(blend_file)
        blend_data.save("blendomatic.dat", self.metadata_formats)

        # TODO: blend_data -> metadata dump

    def read_langfiles(self):
        from .stringresource import StringResource
        from .pefile import PEFile
        from .hdlanguagefile import HDLanguageFile

        stringres = StringResource()

        for langdll in vd.get_files("langdll"):
            stringres.fill_from(PEFile(langdll))

        for langhd in vd.get_files("langhd"):
            stringres.fill_from(PEFile(langhd))

        # TODO: transform and cleanup the read strings...
        #       (strip html, insert formatchars/identifiers, ...)

        # TODO: stringres -> metadata dump

    def read_gamedata_cache(self, cachefile):
        import pickle

        if self.args.use_dat_cache:
            dbg("trying to use cache file %s..." % (cachefile), lvl=1)
            try:
                with open(cachefile, 'rb') as f:
                    gamedata = pickle.load(f)
                    dbg("could successfully load cached gamedata!", lvl=1)
                    return gamedata

            except FileNotFoundError as err:
                return None

        return None

    def write_gamedata_cache(self, cachefile, gamedata):
        import pickle

        if self.args.use_dat_cache:
            with open(cachefile, 'wb') as f:
                pickle.dump(gamedata, f)

    def read_gamedata(self):
        from .gamedata.empiresdat import EmpiresDatGzip, EmpiresDatWrapper
        from .fix_data import fix_data
        from tempfile import gettempdir

        filepath = self.vd.get_files('dat').pop()
        filename = os.path.split(filepath)[1]
        filetrunk = os.path.splitext(filename)[0]

        cachefile = os.path.join(gettempdir(), filename + '.pickle')

        gamedata = self.read_gamedata_cache(cachefile)

        if gamedata is None:
            datfile = EmpiresDatGzip(dat_filepath)
            gamedata = EmpiresDatWrapper()

            if self.args.extrafiles:
                datfile.raw_dump(os.path.join('raw', filetrunk + '.raw'))

            dbg("reading main data file %s..." % (filename), lvl=1)
            gamedata.read(datfile.content, 0)
            self.write_gamedata_cache(cachefile, gamedata)

        dbg("repairing values in main data file %s..." % (filename), lvl=1)
        gamedata.empiresdat[0] = fix_data(gamedata.empiresdat[0])

        # TODO: data transformation (merge stuff, etc.)

        # TODO: gamedata -> metadata dump

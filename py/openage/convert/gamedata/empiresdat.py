# Copyright 2013-2015 the openage authors. See copying.md for legal info.

import zlib

from . import civ
from . import graphic
from . import playercolor
from . import research
from . import sound
from . import tech
from . import terrain
from . import unit

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN
from openage.log import dbg


# this file can parse and represent the empires2_x1_p1.dat file.
#
# the dat file contain all the information needed for running the game.
# all units, buildings, terrains, whatever are defined in this dat file.
#
# documentation for this can be found in `doc/gamedata`
# the binary structure, which the dat file has, is in `doc/gamedata.struct`


class EmpiresDatGzip:
    """
    uncompresses the gzip'd empires dat.
    """

    def __init__(self, fname):
        self.fname = fname
        dbg("reading empires2*.dat from %s..." % fname, lvl=1)

        f = open(fname, "rb")

        dbg("decompressing data from %s" % fname, lvl=2)

        compressed_data = f.read()
        # decompress content with zlib (note the magic -15)
        # -15: - -> there is no header, 15 is the max windowsize
        self.content = zlib.decompress(compressed_data, -15)
        f.close()

        self.compressed_size   = len(compressed_data)
        self.decompressed_size = len(self.content)

        # compressed data no longer needed
        del compressed_data

        dbg("length of compressed data: %d = %d kB" % (self.compressed_size, self.compressed_size / 1024), lvl=2)
        dbg("length of decompressed data: %d = %d kB" % (self.decompressed_size, self.decompressed_size / 1024), lvl=2)

    def raw_dump(self, fname):
        """
        save the dat file in uncompressed format.
        """

        dbg("saving uncompressed %s file to %s" % (self.fname, fname), 1)
        with open(fname, "rb") as f:
            f.write(self.content)


class EmpiresDat(Exportable):
    """
    class for fighting and beating the compressed empires2*.dat

    represents the main game data file.
    """

    name_struct_file   = "gamedata"
    name_struct        = "empiresdat"
    struct_description = "empires2_x1_p1.dat structure"

    data_format = (
        (READ, "versionstr", "char[8]"),

        # terain header data
        (READ, "terrain_restriction_count", "uint16_t"),
        (READ, "terrain_count", "uint16_t"),
        (READ, "terrain_restriction_offset0", "int32_t[terrain_restriction_count]"),
        (READ, "terrain_restriction_offset1", "int32_t[terrain_restriction_count]"),
        (READ, "terrain_restrictions", SubdataMember(
            ref_type=terrain.TerrainRestriction,
            length="terrain_restriction_count",
            passed_args={"terrain_count"},
        )),

        # player color data
        (READ, "player_color_count", "uint16_t"),
        (READ, "player_colors", SubdataMember(
            ref_type=playercolor.PlayerColor,
            length="player_color_count",
        )),

        # sound data
        (READ_EXPORT, "sound_count", "uint16_t"),
        (READ_EXPORT, "sounds", SubdataMember(
            ref_type=sound.Sound,
            length="sound_count",
        )),

        # graphic data
        (READ, "graphic_count", "uint16_t"),
        (READ, "graphic_offsets", "int32_t[graphic_count]"),
        (READ_EXPORT, "graphics", SubdataMember(
            ref_type  = graphic.Graphic,
            length    = "graphic_count",
            offset_to = ("graphic_offsets", lambda o: o > 0),
        )),
        (READ_UNKNOWN, "rendering_blob", "uint8_t[138]"),

        # terrain data
        (READ_EXPORT,  "terrains", SubdataMember(
            ref_type=terrain.Terrain,
            length="terrain_count",
        )),
        (READ_UNKNOWN, "terrain_blob0", "uint8_t[438]"),
        (READ,         "terrain_border", SubdataMember(
            ref_type=terrain.TerrainBorder,
            length=16,
        )),
        (READ_UNKNOWN, "zero", "int8_t[28]"),
        (READ,         "terrain_count_additional", "uint16_t"),
        (READ_UNKNOWN, "terrain_blob1", "uint8_t[12722]"),

        # technology data
        (READ_EXPORT, "tech_count", "uint32_t"),
        (READ_EXPORT, "techs", SubdataMember(
            ref_type=tech.Tech,
            length="tech_count",
        )),

        # unit header data
        (READ_EXPORT, "unit_count", "uint32_t"),
        (READ_EXPORT, "unit_headers", SubdataMember(
            ref_type=unit.UnitHeader,
            length="unit_count",
        )),

        # civilisation data
        (READ_EXPORT, "civ_count", "uint16_t"),
        (READ_EXPORT, "civs", SubdataMember(
            ref_type=civ.Civ,
            length="civ_count"
        )),

        # research data
        (READ_EXPORT, "research_count", "uint16_t"),
        (READ_EXPORT, "researches", SubdataMember(
            ref_type=research.Research,
            length="research_count"
        )),

        # unknown shiat again
        (READ_UNKNOWN, None, "uint32_t[7]"),

        # technology tree data
        (READ_EXPORT, "age_entry_count", "uint8_t"),
        (READ_EXPORT, "building_connection_count", "uint8_t"),
        (READ_EXPORT, "unit_connection_count", "uint8_t"),
        (READ_EXPORT, "research_connection_count", "uint8_t"),
        (READ_EXPORT, "age_tech_tree", SubdataMember(
            ref_type=tech.AgeTechTree,
            length="age_entry_count"
        )),
        (READ_UNKNOWN, None, "uint32_t"),
        (READ_EXPORT, "building_connection", SubdataMember(
            ref_type=tech.BuildingConnection,
            length="building_connection_count"
        )),
        (READ_EXPORT, "unit_connection", SubdataMember(
            ref_type=tech.UnitConnection,
            length="unit_connection_count"
        )),
        (READ_EXPORT, "research_connection", SubdataMember(
            ref_type=tech.ResearchConnection,
            length="research_connection_count"
        )),
    )


class EmpiresDatWrapper(Exportable):
    name_struct_file   = "gamedata"
    name_struct        = "gamedata"
    struct_description = "wrapper for empires2_x1_p1.dat structure"

    # TODO: we could reference to other gamedata structures
    data_format = (
        (READ_EXPORT, "empiresdat", SubdataMember(
            ref_type=EmpiresDat,
            length=1,
        )),
    )

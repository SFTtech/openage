# Copyright 2013-2014 the openage authors. See copying.md for legal info.

#define little endian byte order for the dat file
endianness = '< '

from . import civ
from . import graphic
from . import playercolor
from . import research
from . import sound
from . import tech
from . import terrain
from . import unit

from .. import dataformat
from struct import Struct, unpack_from
from ..util import dbg, file_get_path, file_open, file_write, zstr
import zlib


# this file can parse and represent the empires2_x1_p1.dat file,
# and also its predecessors, empires2.dat and empires2_x1.dat.
#
# these files contain all the information needed for running the game.
# all units, buildings, terrains, whatever are defined in this dat file.
#
# documentation for this can be found in `doc/gamedata`
# the binary structure, which the dat file has, is in `doc/gamedata.struct`


class EmpiresDatGzip:
    """
    uncompresses the gzip'd empires dat.
    """

    def __init__(self, datfile_name):
        self.fname = datfile_name
        dbg("reading empires2*.dat from %s..." % self.fname, lvl=1)

        filename = file_get_path(self.fname, write=False)
        f = file_open(filename, binary=True, write=False)

        dbg("decompressing data from %s" % filename, lvl=2)

        compressed_data = f.read()
        #decompress content with zlib (note the magic -15)
        #-15: - -> there is no header, 15 is the max windowsize
        self.content = zlib.decompress(compressed_data, -15)
        f.close()

        self.compressed_size   = len(compressed_data)
        self.decompressed_size = len(self.content)

        #compressed data no longer needed
        del compressed_data

        dbg("length of compressed data: %d = %d kB" % (self.compressed_size, self.compressed_size/1024), lvl=2)
        dbg("length of decompressed data: %d = %d kB" % (self.decompressed_size, self.decompressed_size/1024), lvl=2)

    def raw_dump(self, filename):
        """
        save the dat file in uncompressed format.
        """

        rawfile_writepath = file_get_path(filename, write=True)
        dbg("saving uncompressed %s file to %s" % (self.fname, rawfile_writepath), 1)
        file_write(rawfile_writepath, self.content)


class EmpiresDat(dataformat.Exportable):
    """
    class for fighting and beating the compressed empires2*.dat

    represents the main game data file.
    """

    name_struct_file   = "gamedata"
    name_struct        = "empiresdat"
    struct_description = "empires2_x1_p1.dat structure"

    data_format = (
        (dataformat.READ, "versionstr", "char[8]"),

        #terain header data
        (dataformat.READ, "terrain_restriction_count", "uint16_t"),
        (dataformat.READ, "terrain_count", "uint16_t"),
        (dataformat.READ, "terrain_restriction_offset0", "int32_t[terrain_restriction_count]"),
        (dataformat.READ, "terrain_restriction_offset1", "int32_t[terrain_restriction_count]"),
        (dataformat.READ, "terrain_restrictions", dataformat.SubdataMember(
            ref_type=terrain.TerrainRestriction,
            length="terrain_restriction_count",
            passed_args={"terrain_count"},
        )),

        #player color data
        (dataformat.READ, "player_color_count", "uint16_t"),
        (dataformat.READ, "player_colors", dataformat.SubdataMember(
            ref_type=playercolor.PlayerColor,
            length="player_color_count",
        )),

        #sound data
        (dataformat.READ_EXPORT, "sound_count", "uint16_t"),
        (dataformat.READ_EXPORT, "sounds", dataformat.SubdataMember(
            ref_type=sound.Sound,
            length="sound_count",
        )),

        #graphic data
        (dataformat.READ, "graphic_count", "uint16_t"),
        (dataformat.READ, "graphic_offsets", "int32_t[graphic_count]"),
        (dataformat.READ_EXPORT, "graphics", dataformat.SubdataMember(
            ref_type  = graphic.Graphic,
            length    = "graphic_count",
            offset_to = ("graphic_offsets", lambda o: o > 0),
        )),
        (dataformat.READ_UNKNOWN, "rendering_blob", "uint8_t[138]"),

        #terrain data
        (dataformat.READ_EXPORT,  "terrains", dataformat.SubdataMember(
            ref_type=terrain.Terrain,
            length="terrain_count",
        )),
        (dataformat.READ_UNKNOWN, "terrain_blob0", "uint8_t[438]"),
        (dataformat.READ,         "terrain_border", dataformat.SubdataMember(
            ref_type=terrain.TerrainBorder,
            length=16,
        )),
        (dataformat.READ_UNKNOWN, "zero", "int8_t[28]"),
        (dataformat.READ,         "terrain_count_additional", "uint16_t"),
        (dataformat.READ_UNKNOWN, "terrain_blob1", "uint8_t[12722]"),

        #technology data
        (dataformat.READ_EXPORT, "tech_count", "uint32_t"),
        (dataformat.READ_EXPORT, "techs", dataformat.SubdataMember(
            ref_type=tech.Tech,
            length="tech_count",
        )),

        #unit header data
        (dataformat.READ_EXPORT, "unit_count", "uint32_t"),
        (dataformat.READ_EXPORT, "unit_headers", dataformat.SubdataMember(
            ref_type=unit.UnitHeader,
            length="unit_count",
        )),

        #civilisation data
        (dataformat.READ_EXPORT, "civ_count", "uint16_t"),
        (dataformat.READ_EXPORT, "civs", dataformat.SubdataMember(
            ref_type=civ.Civ,
            length="civ_count"
        )),

        #research data
        (dataformat.READ_EXPORT, "research_count", "uint16_t"),
        (dataformat.READ_EXPORT, "researches", dataformat.SubdataMember(
            ref_type=research.Research,
            length="research_count"
        )),

        #unknown shiat again
        (dataformat.READ_UNKNOWN, None, "uint32_t[7]"),

        #technology tree data
        (dataformat.READ_EXPORT, "age_entry_count", "uint8_t"),
        (dataformat.READ_EXPORT, "building_connection_count", "uint8_t"),
        (dataformat.READ_EXPORT, "unit_connection_count", "uint8_t"),
        (dataformat.READ_EXPORT, "research_connection_count", "uint8_t"),
        (dataformat.READ_EXPORT, "age_tech_tree", dataformat.SubdataMember(
            ref_type=tech.AgeTechTree,
            length="age_entry_count"
        )),
        (dataformat.READ_UNKNOWN, None, "uint32_t"),
        (dataformat.READ_EXPORT, "building_connection", dataformat.SubdataMember(
            ref_type=tech.BuildingConnection,
            length="building_connection_count"
        )),
        (dataformat.READ_EXPORT, "unit_connection", dataformat.SubdataMember(
            ref_type=tech.UnitConnection,
            length="unit_connection_count"
        )),
        (dataformat.READ_EXPORT, "research_connection", dataformat.SubdataMember(
            ref_type=tech.ResearchConnection,
            length="research_connection_count"
        )),
    )


class EmpiresDatWrapper(dataformat.Exportable):
    name_struct_file   = "gamedata"
    name_struct        = "gamedata"
    struct_description = "wrapper for empires2_x1_p1.dat structure"

    #TODO: we could reference to other gamedata structures
    data_format = (
        (dataformat.READ_EXPORT, "empiresdat", dataformat.SubdataMember(
            ref_type=EmpiresDat,
            length=1,
        )),
    )

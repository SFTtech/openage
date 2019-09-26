# Copyright 2013-2018 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

import pickle
from zlib import decompress

from . import civ
from . import graphic
from . import maps
from . import playercolor
from . import research
from . import sound
from . import tech
from . import terrain
from . import unit

from ..game_versions import GameVersion
from ..dataformat.exportable import Exportable
from openage.convert.dataformat.read_members import SubdataMember
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN

from ...log import spam, dbg, info, warn


# this file can parse and represent the empires2_x1_p1.dat file.
#
# the dat file contain all the information needed for running the game.
# all units, buildings, terrains, whatever are defined in this dat file.
#
# documentation for this can be found in `doc/gamedata`
# the binary structure, which the dat file has, is in `doc/gamedata.struct`


class EmpiresDat(Exportable):
    """
    class for fighting and beating the compressed empires2*.dat

    represents the main game data file.
    """

    name_struct_file   = "gamedata"
    name_struct        = "empiresdat"
    struct_description = "empires2_x1_p1.dat structure"

    data_format = [(READ, "versionstr", "char[8]")]

    # TODO: Enable conversion for SWGB
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.extend([
    #         (READ, "civ_count_swgb", "uint16_t"),
    #         (READ_UNKNOWN, None, "int32_t"),
    #         (READ_UNKNOWN, None, "int32_t"),
    #         (READ_UNKNOWN, None, "int32_t"),
    #         (READ_UNKNOWN, None, "int32_t"),
    #     ])
    # ===========================================================================

    # terrain header data
    data_format.extend([
        (READ, "terrain_restriction_count", "uint16_t"),
        (READ, "terrain_count", "uint16_t"),   # number of "used" terrains
        (READ, "float_ptr_terrain_tables", "int32_t[terrain_restriction_count]"),
    ])

    # TODO: Enable conversion for AOE1; replace "terrain_pass_graphics_ptrs"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ, "terrain_pass_graphics_ptrs", "int32_t[terrain_restriction_count]"))
    # ===========================================================================
    data_format.append((READ, "terrain_pass_graphics_ptrs", "int32_t[terrain_restriction_count]"))

    data_format.extend([
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
        (READ, "graphic_ptrs", "uint32_t[graphic_count]"),
        (READ_EXPORT, "graphics", SubdataMember(
            ref_type  = graphic.Graphic,
            length    = "graphic_count",
            offset_to = ("graphic_ptrs", lambda o: o > 0),
        )),

        # terrain data
        (READ, "virt_function_ptr", "int32_t"),
        (READ, "map_pointer", "int32_t"),
        (READ, "map_width", "int32_t"),
        (READ, "map_height", "int32_t"),
        (READ, "world_width", "int32_t"),
        (READ, "world_height", "int32_t"),
        (READ_EXPORT,  "tile_sizes", SubdataMember(
            ref_type=terrain.TileSize,
            length=19,      # number of tile types
        )),
        (READ, "padding1", "int16_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "terrains"
    # ===========================================================================
    # # 42 terrains are stored (100 in African Kingdoms), but less are used.
    # # TODO: maybe this number is defined somewhere.
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_EXPORT,  "terrains", SubdataMember(
    #             ref_type=terrain.Terrain,
    #             length=55,
    #             )))
    # elif GameVersion.age2_hd_ak in game_versions:
    #     data_format.append((READ_EXPORT,  "terrains", SubdataMember(
    #             ref_type=terrain.Terrain,
    #             length=100,
    #             )))
    # elif (GameVersion.aoe_1 or GameVersion.aoe_ror) in game_versions:
    #     data_format.append((READ_EXPORT,  "terrains", SubdataMember(
    #             ref_type=terrain.Terrain,
    #             length=42,
    #             )))
    # else:
    #     data_format.append((READ_EXPORT,  "terrains", SubdataMember(
    #             ref_type=terrain.Terrain,
    #             length=42,
    #             )))
    # ===========================================================================
    data_format.append(
        (READ_EXPORT,  "terrains", SubdataMember(
            ref_type=terrain.Terrain,
            # 42 terrains are stored (100 in African Kingdoms), but less are used.
            # TODO: maybe this number is defined somewhere.
            length=(lambda self:
                    100 if GameVersion.age2_hd_ak in self.game_versions
                    else 42),
        )))

    data_format.extend([
        (READ,         "terrain_border", SubdataMember(
            ref_type=terrain.TerrainBorder,
            length=16,
        )),

        (READ,         "map_row_offset", "int32_t"),
        (READ,         "map_min_x", "float"),
        (READ,         "map_min_y", "float"),
        (READ,         "map_max_x", "float"),
        (READ,         "map_max_y", "float"),
        (READ,         "map_max_xplus1", "float"),
        (READ,         "map_min_yplus1", "float"),

        (READ,         "terrain_count_additional", "uint16_t"),
        (READ,         "borders_used", "uint16_t"),
        (READ,         "max_terrain", "int16_t"),
        (READ_EXPORT,  "tile_width", "int16_t"),
        (READ_EXPORT,  "tile_height", "int16_t"),
        (READ_EXPORT,  "tile_half_height", "int16_t"),
        (READ_EXPORT,  "tile_half_width", "int16_t"),
        (READ_EXPORT,  "elev_height", "int16_t"),
        (READ,         "current_row", "int16_t"),
        (READ,         "current_column", "int16_t"),
        (READ,         "block_beginn_row", "int16_t"),
        (READ,         "block_end_row", "int16_t"),
        (READ,         "block_begin_column", "int16_t"),
        (READ,         "block_end_column", "int16_t"),
        (READ,         "search_map_ptr", "int32_t"),
        (READ,         "search_map_rows_ptr", "int32_t"),
        (READ,         "any_frame_change", "int8_t"),
        (READ,         "map_visible_flag", "int8_t"),
        (READ,         "fog_flag", "int8_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "terrain_blob0"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_UNKNOWN, "terrain_blob0", "uint8_t[25]"))
    # else:
    #     data_format.append((READ_UNKNOWN, "terrain_blob0", "uint8_t[21]"))
    # ===========================================================================
    data_format.append((READ_UNKNOWN, "terrain_blob0", "uint8_t[21]"))

    data_format.extend([
        (READ_UNKNOWN, "terrain_blob1", "uint32_t[157]"),

        # random map config
        (READ, "random_map_count", "uint32_t"),
        (READ, "random_map_ptr", "uint32_t"),
        (READ, "map_infos", SubdataMember(
            ref_type=maps.MapInfo,
            length="random_map_count",
        )),
        (READ, "maps", SubdataMember(
            ref_type=maps.Map,
            length="random_map_count",
        )),

        # technology data
        (READ_EXPORT, "tech_count", "uint32_t"),
        (READ_EXPORT, "techs", SubdataMember(
            ref_type=tech.Tech,
            length="tech_count",
        )),
    ])

    # TODO: Enable conversion for SWGB
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.extend([
    #         (READ, "unit_line_count", "uint16_t"),
    #         (READ, "unit_lines", SubdataMember(
    #             ref_type=unit.UnitLine,
    #             length="unit_line_count",
    #         )),
    #     ])
    # ===========================================================================

    # unit header data
    # TODO: Enable conversion for AOE1; replace "unit_count", "unit_headers"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([(READ_EXPORT, "unit_count", "uint32_t"),
    #                         (READ_EXPORT, "unit_headers", SubdataMember(
    #                             ref_type=unit.UnitHeader,
    #                             length="unit_count",
    #                         )),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ_EXPORT, "unit_count", "uint32_t"),
        (READ_EXPORT, "unit_headers", SubdataMember(
            ref_type=unit.UnitHeader,
            length="unit_count",
        )),
    ])

    # civilisation data
    data_format.extend([
        (READ_EXPORT, "civ_count", "uint16_t"),
        (READ_EXPORT, "civs", SubdataMember(
            ref_type=civ.Civ,
            length="civ_count"
        )),
    ])

    # TODO: Enable conversion for SWGB
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_UNKNOWN, None, "int8_t"))
    # ===========================================================================

    # research data
    data_format.extend([
        (READ_EXPORT, "research_count", "uint16_t"),
        (READ_EXPORT, "researches", SubdataMember(
            ref_type=research.Research,
            length="research_count"
        )),
    ])

    # TODO: Enable conversion for SWGB
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_UNKNOWN, None, "int8_t"))
    # ===========================================================================

    # TODO: Enable conversion for AOE1; replace the 7 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ, "time_slice", "int32_t"),
    #         (READ, "unit_kill_rate", "int32_t"),
    #         (READ, "unit_kill_total", "int32_t"),
    #         (READ, "unit_hitpoint_rate", "int32_t"),
    #         (READ, "unit_hitpoint_total", "int32_t"),
    #         (READ, "razing_kill_rate", "int32_t"),
    #         (READ, "razing_kill_total", "int32_t"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "time_slice", "int32_t"),
        (READ, "unit_kill_rate", "int32_t"),
        (READ, "unit_kill_total", "int32_t"),
        (READ, "unit_hitpoint_rate", "int32_t"),
        (READ, "unit_hitpoint_total", "int32_t"),
        (READ, "razing_kill_rate", "int32_t"),
        (READ, "razing_kill_total", "int32_t"),
    ])
    # ===========================================================================

    # technology tree data
    data_format.extend([
        (READ_EXPORT, "age_entry_count", "uint8_t"),
        (READ_EXPORT, "building_connection_count", "uint8_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "unit_connection_count"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_EXPORT, "unit_connection_count", "uint16_t"))
    # else:
    #     data_format.append((READ_EXPORT, "unit_connection_count", "uint8_t"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "unit_connection_count", "uint8_t"))

    data_format.extend([
        (READ_EXPORT, "research_connection_count", "uint8_t"),
        (READ_EXPORT, "age_tech_tree", SubdataMember(
            ref_type=tech.AgeTechTree,
            length="age_entry_count"
        )),
        # What is this? There shouldn't be something here
        (READ_UNKNOWN, None, "int32_t"),
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
    ])

    @classmethod
    def get_hash(cls):
        """ return the unique hash for the data format tree """

        return cls.format_hash().hexdigest()


class EmpiresDatWrapper(Exportable):
    """
    This wrapper exists because the top-level element is discarded:
    The gathered data fields are passed to the parent,
    and are accumulated there to be written out.

    This class acts as the parent for the "real" data values,
    and has no parent itself. Thereby this class is discarded
    and the child classes use this as parent for their return values.
    """

    name_struct_file   = "gamedata"
    name_struct        = "gamedata"
    struct_description = "wrapper for empires2_x1_p1.dat structure"

    # TODO: we could reference to other gamedata structures
    data_format = [
        (READ_EXPORT, "empiresdat", SubdataMember(
            ref_type=EmpiresDat,
            length=1,
        )),
    ]


def load_gamespec(fileobj, game_versions, cachefile_name=None, load_cache=False):
    """
    Helper method that loads the contents of a 'empires.dat' gzipped gamespec
    file.

    If cachefile_name is given, this file is consulted before performing the
    load.
    """
    # try to use the cached result from a previous run
    if cachefile_name and load_cache:
        try:
            with open(cachefile_name, "rb") as cachefile:
                # pickle.load() can fail in many ways, we need to catch all.
                # pylint: disable=broad-except
                try:
                    gamespec = pickle.load(cachefile)
                    info("using cached gamespec: %s", cachefile_name)
                    return gamespec
                except Exception:
                    warn("could not use cached gamespec:")
                    import traceback
                    traceback.print_exc()
                    warn("we will just skip the cache, no worries.")

        except FileNotFoundError:
            pass

    # read the file ourselves

    dbg("reading dat file")
    compressed_data = fileobj.read()
    fileobj.close()

    dbg("decompressing dat file")
    # -15: there's no header, window size is 15.
    file_data = decompress(compressed_data, -15)
    del compressed_data

    spam("length of decompressed data: %d", len(file_data))

    gamespec = EmpiresDatWrapper(game_versions=game_versions)
    gamespec.read(file_data, 0)

    if cachefile_name:
        dbg("dumping dat file contents to cache file: %s", cachefile_name)
        with open(cachefile_name, "wb") as cachefile:
            pickle.dump(gamespec, cachefile)

    return gamespec

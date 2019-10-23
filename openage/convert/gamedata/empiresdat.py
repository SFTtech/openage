# Copyright 2013-2019 the openage authors. See copying.md for legal info.

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
from ..dataformat.genie_structure import GenieStructure
from ..dataformat.read_members import SubdataMember
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN
from ..dataformat.value_members import MemberTypes as StorageType

from ...log import spam, dbg, info, warn


# this file can parse and represent the empires2_x1_p1.dat file.
#
# the dat file contain all the information needed for running the game.
# all units, buildings, terrains, whatever are defined in this dat file.
#
# documentation for this can be found in `doc/gamedata`
# the binary structure, which the dat file has, is in `doc/gamedata.struct`


class EmpiresDat(GenieStructure):
    """
    class for fighting and beating the compressed empires2*.dat

    represents the main game data file.
    """

    name_struct_file   = "gamedata"
    name_struct        = "empiresdat"
    struct_description = "empires2_x1_p1.dat structure"

    data_format = [(READ, "versionstr", StorageType.STRING_MEMBER, "char[8]")]

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
        (READ, "terrain_restriction_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "terrain_count", StorageType.INT_MEMBER, "uint16_t"),   # number of "used" terrains
        (READ, "float_ptr_terrain_tables", StorageType.ARRAY_ID, "int32_t[terrain_restriction_count]"),
    ])

    # TODO: Enable conversion for AOE1; replace "terrain_pass_graphics_ptrs"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ, "terrain_pass_graphics_ptrs", "int32_t[terrain_restriction_count]"))
    # ===========================================================================
    data_format.append((READ, "terrain_pass_graphics_ptrs", StorageType.ARRAY_ID, "int32_t[terrain_restriction_count]"))

    data_format.extend([
        (READ, "terrain_restrictions", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=terrain.TerrainRestriction,
            length="terrain_restriction_count",
            passed_args={"terrain_count"},
        )),

        # player color data
        (READ, "player_color_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "player_colors", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=playercolor.PlayerColor,
            length="player_color_count",
        )),

        # sound data
        (READ_EXPORT, "sound_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ_EXPORT, "sounds", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=sound.Sound,
            length="sound_count",
        )),

        # graphic data
        (READ, "graphic_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "graphic_ptrs", StorageType.ARRAY_ID, "uint32_t[graphic_count]"),
        (READ_EXPORT, "graphics", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type  = graphic.Graphic,
            length    = "graphic_count",
            offset_to = ("graphic_ptrs", lambda o: o > 0),
        )),

        # terrain data
        (READ, "virt_function_ptr", StorageType.ID_MEMBER, "int32_t"),
        (READ, "map_pointer", StorageType.ID_MEMBER, "int32_t"),
        (READ, "map_width", StorageType.INT_MEMBER, "int32_t"),
        (READ, "map_height", StorageType.INT_MEMBER, "int32_t"),
        (READ, "world_width", StorageType.INT_MEMBER, "int32_t"),
        (READ, "world_height", StorageType.INT_MEMBER, "int32_t"),
        (READ_EXPORT,  "tile_sizes", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=terrain.TileSize,
            length=19,      # number of tile types
        )),
        (READ, "padding1", StorageType.INT_MEMBER, "int16_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "terrains"
    # ===========================================================================
    # # 42 terrains are stored (100 in African Kingdoms), but less are used.
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
        (READ_EXPORT,  "terrains", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=terrain.Terrain,
            # 42 terrains are stored (100 in African Kingdoms), but less are used.
            length=(lambda self:
                    100 if GameVersion.age2_hd_ak in self.game_versions
                    else 42),
        )))

    data_format.extend([
        (READ,         "terrain_border", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=terrain.TerrainBorder,
            length=16,
        )),

        (READ,         "map_row_offset", StorageType.INT_MEMBER, "int32_t"),
        (READ,         "map_min_x", StorageType.FLOAT_MEMBER, "float"),
        (READ,         "map_min_y", StorageType.FLOAT_MEMBER, "float"),
        (READ,         "map_max_x", StorageType.FLOAT_MEMBER, "float"),
        (READ,         "map_max_y", StorageType.FLOAT_MEMBER, "float"),
        (READ,         "map_max_xplus1", StorageType.FLOAT_MEMBER, "float"),
        (READ,         "map_min_yplus1", StorageType.FLOAT_MEMBER, "float"),

        (READ,         "terrain_count_additional", StorageType.INT_MEMBER, "uint16_t"),
        (READ,         "borders_used", StorageType.INT_MEMBER, "uint16_t"),
        (READ,         "max_terrain", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT,  "tile_width", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT,  "tile_height", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT,  "tile_half_height", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT,  "tile_half_width", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT,  "elev_height", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "current_row", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "current_column", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "block_beginn_row", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "block_end_row", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "block_begin_column", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "block_end_column", StorageType.INT_MEMBER, "int16_t"),
        (READ,         "search_map_ptr", StorageType.INT_MEMBER, "int32_t"),
        (READ,         "search_map_rows_ptr", StorageType.INT_MEMBER, "int32_t"),
        (READ,         "any_frame_change", StorageType.INT_MEMBER, "int8_t"),
        (READ,         "map_visible_flag", StorageType.INT_MEMBER, "int8_t"),
        (READ,         "fog_flag", StorageType.INT_MEMBER, "int8_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "terrain_blob0"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_UNKNOWN, "terrain_blob0", "uint8_t[25]"))
    # else:
    #     data_format.append((READ_UNKNOWN, "terrain_blob0", "uint8_t[21]"))
    # ===========================================================================
    data_format.append((READ_UNKNOWN, "terrain_blob0", StorageType.ARRAY_INT, "uint8_t[21]"))

    data_format.extend([
        (READ_UNKNOWN, "terrain_blob1", StorageType.ARRAY_INT, "uint32_t[157]"),

        # random map config
        (READ, "random_map_count", StorageType.INT_MEMBER, "uint32_t"),
        (READ, "random_map_ptr", StorageType.ID_MEMBER, "uint32_t"),
        (READ, "map_infos", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=maps.MapInfo,
            length="random_map_count",
        )),
        (READ, "maps", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=maps.Map,
            length="random_map_count",
        )),

        # technology effect data
        (READ_EXPORT, "effect_bundle_count", StorageType.INT_MEMBER, "uint32_t"),
        (READ_EXPORT, "effect_bundles", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=tech.EffectBundle,
            length="effect_bundle_count",
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
        (READ_EXPORT, "unit_count", StorageType.INT_MEMBER, "uint32_t"),
        (READ_EXPORT, "unit_headers", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=unit.UnitHeader,
            length="unit_count",
        )),
    ])

    # civilisation data
    data_format.extend([
        (READ_EXPORT, "civ_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ_EXPORT, "civs", StorageType.ARRAY_CONTAINER, SubdataMember(
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
        (READ_EXPORT, "research_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ_EXPORT, "researches", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=research.Tech,
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
        (READ, "time_slice", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_kill_rate", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_kill_total", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_hitpoint_rate", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_hitpoint_total", StorageType.INT_MEMBER, "int32_t"),
        (READ, "razing_kill_rate", StorageType.INT_MEMBER, "int32_t"),
        (READ, "razing_kill_total", StorageType.INT_MEMBER, "int32_t"),
    ])
    # ===========================================================================

    # technology tree data
    data_format.extend([
        (READ_EXPORT, "age_connection_count", StorageType.INT_MEMBER, "uint8_t"),
        (READ_EXPORT, "building_connection_count", StorageType.INT_MEMBER, "uint8_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "unit_connection_count"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_EXPORT, "unit_connection_count", "uint16_t"))
    # else:
    #     data_format.append((READ_EXPORT, "unit_connection_count", "uint8_t"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "unit_connection_count", StorageType.INT_MEMBER, "uint8_t"))

    data_format.extend([
        (READ_EXPORT, "tech_connection_count", StorageType.INT_MEMBER, "uint8_t"),
        (READ_EXPORT, "total_unit_tech_groups", StorageType.INT_MEMBER, "int32_t"),
        (READ_EXPORT, "age_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=tech.AgeTechTree,
            length="age_connection_count"
        )),
        # What is this? There shouldn't be something here
        # (READ_UNKNOWN, None, "int32_t"),
        (READ_EXPORT, "building_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=tech.BuildingConnection,
            length="building_connection_count"
        )),
        (READ_EXPORT, "unit_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=tech.UnitConnection,
            length="unit_connection_count"
        )),
        (READ_EXPORT, "tech_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=tech.ResearchConnection,
            length="tech_connection_count"
        )),
    ])

    @classmethod
    def get_hash(cls):
        """ return the unique hash for the data format tree """

        return cls.format_hash().hexdigest()


class EmpiresDatWrapper(GenieStructure):
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
        (READ_EXPORT, "empiresdat", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=EmpiresDat,
            length=1,
        )),
    ]


def load_gamespec(fileobj, game_versions, cachefile_name=None, load_cache=False):
    """
    Helper method that loads the contents of a 'empires.dat' gzipped wrapper
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
                    wrapper = pickle.load(cachefile)
                    info("using cached wrapper: %s", cachefile_name)
                    return wrapper
                except Exception:
                    warn("could not use cached wrapper:")
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

    wrapper = EmpiresDatWrapper(game_versions=game_versions)
    _, gamespec = wrapper.read(file_data, 0)

    # Remove the list sorrounding the converted data
    gamespec = gamespec[0]

    if cachefile_name:
        dbg("dumping dat file contents to cache file: %s", cachefile_name)
        with open(cachefile_name, "wb") as cachefile:
            pickle.dump(gamespec, cachefile)

    return gamespec

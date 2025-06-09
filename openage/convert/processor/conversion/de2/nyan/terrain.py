# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert terrain groups to openage terrains.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.combined_terrain import CombinedTerrain
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_terrain import GenieTerrainGroup


@staticmethod
def terrain_group_to_terrain(terrain_group: GenieTerrainGroup) -> None:
    """
    Creates raw API objects for a terrain group.

    :param terrain_group: Terrain group that gets converted to a tech.
    """
    terrain_index = terrain_group.get_id()

    dataset = terrain_group.data

    # name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    terrain_lookup_dict = internal_name_lookups.get_terrain_lookups(dataset.game_version)
    terrain_type_lookup_dict = internal_name_lookups.get_terrain_type_lookups(
        dataset.game_version)

    if terrain_index not in terrain_lookup_dict:
        # TODO: Not all terrains are used in DE2; filter out the unused terrains
        # in pre-processor
        return

    # Start with the Terrain object
    terrain_name = terrain_lookup_dict[terrain_index][1]
    raw_api_object = RawAPIObject(terrain_name, terrain_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.terrain.Terrain")
    obj_location = f"data/terrain/{terrain_lookup_dict[terrain_index][2]}/"
    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(terrain_lookup_dict[terrain_index][2])
    terrain_group.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Types
    # =======================================================================
    terrain_types = []

    for terrain_type in terrain_type_lookup_dict.values():
        if terrain_index in terrain_type[0]:
            type_name = f"util.terrain_type.types.{terrain_type[2]}"
            type_obj = dataset.pregen_nyan_objects[type_name].get_nyan_object()
            terrain_types.append(type_obj)

    raw_api_object.add_raw_member("types", terrain_types, "engine.util.terrain.Terrain")

    # =======================================================================
    # Name
    # =======================================================================
    name_ref = f"{terrain_name}.{terrain_name}Name"
    name_raw_api_object = RawAPIObject(name_ref,
                                       f"{terrain_name}Name",
                                       dataset.nyan_api_objects)
    name_raw_api_object.add_raw_parent("engine.util.language.translated.type.TranslatedString")
    name_location = ForwardRef(terrain_group, terrain_name)
    name_raw_api_object.set_location(name_location)

    name_raw_api_object.add_raw_member("translations",
                                       [],
                                       "engine.util.language.translated.type.TranslatedString")

    name_forward_ref = ForwardRef(terrain_group, name_ref)
    raw_api_object.add_raw_member("name", name_forward_ref, "engine.util.terrain.Terrain")
    terrain_group.add_raw_api_object(name_raw_api_object)

    # =======================================================================
    # Sound
    # =======================================================================
    sound_name = f"{terrain_name}.Sound"
    sound_raw_api_object = RawAPIObject(sound_name, "Sound",
                                        dataset.nyan_api_objects)
    sound_raw_api_object.add_raw_parent("engine.util.sound.Sound")
    sound_location = ForwardRef(terrain_group, terrain_name)
    sound_raw_api_object.set_location(sound_location)

    # TODO: Sounds
    sounds = []

    sound_raw_api_object.add_raw_member("play_delay",
                                        0,
                                        "engine.util.sound.Sound")
    sound_raw_api_object.add_raw_member("sounds",
                                        sounds,
                                        "engine.util.sound.Sound")

    sound_forward_ref = ForwardRef(terrain_group, sound_name)
    raw_api_object.add_raw_member("sound",
                                  sound_forward_ref,
                                  "engine.util.terrain.Terrain")

    terrain_group.add_raw_api_object(sound_raw_api_object)

    # =======================================================================
    # Ambience
    # =======================================================================
    terrain = terrain_group.get_terrain()
    # ambients_count = terrain["terrain_units_used_count"].value

    ambience = []
    # TODO: Ambience
# ===============================================================================
#     for ambient_index in range(ambients_count):
#         ambient_id = terrain["terrain_unit_id"][ambient_index].value
#
#         if ambient_id == -1:
#             continue
#
#         ambient_line = dataset.unit_ref[ambient_id]
#         ambient_name = name_lookup_dict[ambient_line.get_head_unit_id()][0]
#
#         ambient_ref = "%s.Ambient%s" % (terrain_name, str(ambient_index))
#         ambient_raw_api_object = RawAPIObject(ambient_ref,
#                                               "Ambient%s" % (str(ambient_index)),
#                                               dataset.nyan_api_objects)
#         ambient_raw_api_object.add_raw_parent("engine.util.terrain.TerrainAmbient")
#         ambient_location = ForwardRef(terrain_group, terrain_name)
#         ambient_raw_api_object.set_location(ambient_location)
#
#         # Game entity reference
#         ambient_line_forward_ref = ForwardRef(ambient_line, ambient_name)
#         ambient_raw_api_object.add_raw_member("object",
#                                               ambient_line_forward_ref,
#                                               "engine.util.terrain.TerrainAmbient")
#
#         # Max density
#         max_density = terrain["terrain_unit_density"][ambient_index].value
#         ambient_raw_api_object.add_raw_member("max_density",
#                                               max_density,
#                                               "engine.util.terrain.TerrainAmbient")
#
#         terrain_group.add_raw_api_object(ambient_raw_api_object)
#         terrain_ambient_forward_ref = ForwardRef(terrain_group, ambient_ref)
#         ambience.append(terrain_ambient_forward_ref)
# ===============================================================================

    raw_api_object.add_raw_member("ambience", ambience, "engine.util.terrain.Terrain")

    # =======================================================================
    # Path Costs
    # =======================================================================
    path_costs = {}
    restrictions = dataset.genie_terrain_restrictions

    # Land grid
    path_type = dataset.pregen_nyan_objects["util.path.types.Land"].get_nyan_object()
    land_restrictions = restrictions[0x07]
    if land_restrictions.is_accessible(terrain_index):
        path_costs[path_type] = 1

    else:
        path_costs[path_type] = 255

    # Water grid
    path_type = dataset.pregen_nyan_objects["util.path.types.Water"].get_nyan_object()
    water_restrictions = restrictions[0x03]
    if water_restrictions.is_accessible(terrain_index):
        path_costs[path_type] = 1

    else:
        path_costs[path_type] = 255

    # Air grid (default accessible)
    path_type = dataset.pregen_nyan_objects["util.path.types.Air"].get_nyan_object()
    path_costs[path_type] = 1

    raw_api_object.add_raw_member("path_costs", path_costs, "engine.util.terrain.Terrain")

    # =======================================================================
    # Graphic
    # =======================================================================
    texture_id = terrain.get_id()

    # Create animation object
    graphic_name = f"{terrain_name}.TerrainTexture"
    graphic_raw_api_object = RawAPIObject(graphic_name, "TerrainTexture",
                                          dataset.nyan_api_objects)
    graphic_raw_api_object.add_raw_parent("engine.util.graphics.Terrain")
    graphic_location = ForwardRef(terrain_group, terrain_name)
    graphic_raw_api_object.set_location(graphic_location)

    if texture_id in dataset.combined_terrains.keys():
        terrain_graphic = dataset.combined_terrains[texture_id]

    else:
        terrain_graphic = CombinedTerrain(texture_id,
                                          f"texture_{terrain_lookup_dict[terrain_index][2]}",
                                          dataset)
        dataset.combined_terrains.update({terrain_graphic.get_id(): terrain_graphic})

    terrain_graphic.add_reference(graphic_raw_api_object)

    graphic_raw_api_object.add_raw_member("sprite", terrain_graphic,
                                          "engine.util.graphics.Terrain")

    terrain_group.add_raw_api_object(graphic_raw_api_object)
    graphic_forward_ref = ForwardRef(terrain_group, graphic_name)
    raw_api_object.add_raw_member("terrain_graphic", graphic_forward_ref,
                                  "engine.util.terrain.Terrain")

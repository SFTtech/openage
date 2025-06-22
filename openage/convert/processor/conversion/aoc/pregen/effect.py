# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effect objects for AoC.
"""
from __future__ import annotations
import typing

from ......nyan.nyan_structs import MemberSpecialValue
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

ATTRIBUTE_CHANGE_PARENT = "engine.util.attribute_change_type.AttributeChangeType"
CONSTRUCT_PARENT = "engine.util.progress_type.type.Construct"
CONVERT_PARENT = "engine.util.convert_type.ConvertType"
MIN_CHANGE_AMOUNT_PARENT = "engine.util.attribute.AttributeAmount"
MIN_CHANGE_RATE_PARENT = "engine.util.attribute.AttributeRate"
DISCRETE_FLAC_EFFECT_PARENT = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
FALLBACK_EFFECT_PARENT = (
    "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"
)
DISCRETE_FLAC_RESIST_PARENT = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
FALLBACK_RESIST_PARENT = (
    "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"
)
ATTRIBUTE_CHANGE_LOCATION = "data/util/attribute_change_type/"
CONSTRUCT_LOCATION = "data/util/construct_type/"
CONVERT_LOCATION = "data/util/convert_type/"
MIN_CHANGE_LOCATION = "data/effect/discrete/flat_attribute_change/"
FALLBACK_EFFECT_LOCATION = "data/effect/discrete/flat_attribute_change/"
FALLBACK_RESIST_LOCATION = "data/resistance/discrete/flat_attribute_change/"


def generate_effect_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate types for effects and resistances.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    generate_attribute_change_types(full_data_set, pregen_converter_group)
    generate_construct_types(full_data_set, pregen_converter_group)
    generate_convert_types(full_data_set, pregen_converter_group)


def generate_misc_effect_objects(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate fallback types and other standard objects for effects and resistances.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_min_change_values(full_data_set, pregen_converter_group)
    _generate_fallback_effects(full_data_set, pregen_converter_group)
    _generate_fallback_resistances(full_data_set, pregen_converter_group)
    _generate_construct_property(full_data_set, pregen_converter_group)
    _generate_repair_property(full_data_set, pregen_converter_group)


def generate_attribute_change_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the attribute change types for effects and resistances from
    the armor class lookups.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(full_data_set.game_version)
    armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(full_data_set.game_version)

    # =======================================================================
    # Armor classes
    # =======================================================================

    for type_name in armor_lookup_dict.values():
        type_ref_in_modpack = f"util.attribute_change_type.types.{type_name}"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           type_name, api_objects,
                                           ATTRIBUTE_CHANGE_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Heal
    # =======================================================================
    type_ref_in_modpack = "util.attribute_change_type.types.Heal"
    type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                       "Heal", api_objects,
                                       ATTRIBUTE_CHANGE_LOCATION)
    type_raw_api_object.set_filename("types")
    type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

    pregen_converter_group.add_raw_api_object(type_raw_api_object)
    pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Repair (one for each repairable entity)
    # =======================================================================
    repairable_lines = []
    repairable_lines.extend(full_data_set.building_lines.values())
    for unit_line in full_data_set.unit_lines.values():
        if unit_line.is_repairable():
            repairable_lines.append(unit_line)

    for repairable_line in repairable_lines:
        game_entity_name = name_lookup_dict[repairable_line.get_head_unit_id()][0]

        type_ref_in_modpack = f"util.attribute_change_type.types.{game_entity_name}Repair"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           f"{game_entity_name}Repair",
                                           api_objects,
                                           ATTRIBUTE_CHANGE_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Construct (HP changes when constructing a building)
    # =======================================================================
    constructable_lines = []
    constructable_lines.extend(full_data_set.building_lines.values())

    for constructable_line in constructable_lines:
        game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

        type_ref_in_modpack = f"util.attribute_change_type.types.{game_entity_name}Construct"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           f"{game_entity_name}Construct",
                                           api_objects,
                                           ATTRIBUTE_CHANGE_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})


def generate_construct_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the construct types for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(full_data_set.game_version)

    constructable_lines = []
    constructable_lines.extend(full_data_set.building_lines.values())

    for constructable_line in constructable_lines:
        game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

        type_ref_in_modpack = f"util.construct_type.types.{game_entity_name}Construct"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           f"{game_entity_name}Construct",
                                           api_objects,
                                           CONSTRUCT_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(CONSTRUCT_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})


def generate_convert_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the convert types for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Unit conversion
    # =======================================================================

    type_ref_in_modpack = "util.convert_type.types.UnitConvert"
    type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                       "UnitConvert", api_objects,
                                       CONVERT_LOCATION)
    type_raw_api_object.set_filename("types")
    type_raw_api_object.add_raw_parent(CONVERT_PARENT)

    pregen_converter_group.add_raw_api_object(type_raw_api_object)
    pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Building conversion
    # =======================================================================
    type_ref_in_modpack = "util.convert_type.types.BuildingConvert"
    type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                       "BuildingConvert", api_objects,
                                       CONVERT_LOCATION)
    type_raw_api_object.set_filename("types")
    type_raw_api_object.add_raw_parent(CONVERT_PARENT)

    pregen_converter_group.add_raw_api_object(type_raw_api_object)
    pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})


def _generate_min_change_values(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the minimum change values for effects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Min change value (lower cealing for attack effects)
    # =======================================================================
    change_ref_in_modpack = "effect.discrete.flat_attribute_change.min_damage.AoE2MinChangeAmount"
    change_raw_api_object = RawAPIObject(change_ref_in_modpack,
                                         "AoE2MinChangeAmount",
                                         api_objects,
                                         MIN_CHANGE_LOCATION)
    change_raw_api_object.set_filename("min_damage")
    change_raw_api_object.add_raw_parent(MIN_CHANGE_AMOUNT_PARENT)

    attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
    change_raw_api_object.add_raw_member("type",
                                         attribute,
                                         MIN_CHANGE_AMOUNT_PARENT)
    change_raw_api_object.add_raw_member("amount",
                                         0,
                                         MIN_CHANGE_AMOUNT_PARENT)

    pregen_converter_group.add_raw_api_object(change_raw_api_object)
    pregen_nyan_objects.update({change_ref_in_modpack: change_raw_api_object})

    # =======================================================================
    # Min change value (lower cealing for heal effects)
    # =======================================================================
    change_ref_in_modpack = "effect.discrete.flat_attribute_change.min_heal.AoE2MinChangeAmount"
    change_raw_api_object = RawAPIObject(change_ref_in_modpack,
                                         "AoE2MinChangeAmount",
                                         api_objects,
                                         MIN_CHANGE_LOCATION)
    change_raw_api_object.set_filename("min_heal")
    change_raw_api_object.add_raw_parent(MIN_CHANGE_RATE_PARENT)

    attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
    change_raw_api_object.add_raw_member("type",
                                         attribute,
                                         MIN_CHANGE_RATE_PARENT)
    change_raw_api_object.add_raw_member("rate",
                                         0,
                                         MIN_CHANGE_RATE_PARENT)

    pregen_converter_group.add_raw_api_object(change_raw_api_object)
    pregen_nyan_objects.update({change_ref_in_modpack: change_raw_api_object})


def _generate_fallback_effects(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate fallback effects ( = minimum damage) for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    fallback_ref_in_modpack = "effect.discrete.flat_attribute_change.fallback.AoE2AttackFallback"
    fallback_raw_api_object = RawAPIObject(fallback_ref_in_modpack,
                                           "AoE2AttackFallback",
                                           api_objects,
                                           FALLBACK_EFFECT_LOCATION)
    fallback_raw_api_object.set_filename("fallback")
    fallback_raw_api_object.add_raw_parent(FALLBACK_EFFECT_PARENT)

    # Type
    type_ref = "engine.util.attribute_change_type.type.Fallback"
    change_type = api_objects[type_ref]
    fallback_raw_api_object.add_raw_member("type",
                                           change_type,
                                           DISCRETE_FLAC_EFFECT_PARENT)

    # Min value (optional)
    # =================================================================================
    amount_name = f"{fallback_ref_in_modpack}.LowerCealing"
    amount_raw_api_object = RawAPIObject(amount_name, "LowerCealing", api_objects)
    amount_raw_api_object.add_raw_parent("engine.util.attribute.AttributeAmount")
    amount_location = ForwardRef(pregen_converter_group, fallback_ref_in_modpack)
    amount_raw_api_object.set_location(amount_location)

    attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
    amount_raw_api_object.add_raw_member("type",
                                         attribute,
                                         "engine.util.attribute.AttributeAmount")
    amount_raw_api_object.add_raw_member("amount",
                                         1,
                                         "engine.util.attribute.AttributeAmount")

    pregen_converter_group.add_raw_api_object(amount_raw_api_object)
    pregen_nyan_objects.update({amount_name: amount_raw_api_object})
    # =================================================================================
    amount_forward_ref = ForwardRef(pregen_converter_group, amount_name)
    fallback_raw_api_object.add_raw_member("min_change_value",
                                           amount_forward_ref,
                                           DISCRETE_FLAC_EFFECT_PARENT)

    # Max value (optional; not needed

    # Change value
    # =================================================================================
    amount_name = f"{fallback_ref_in_modpack}.ChangeAmount"
    amount_raw_api_object = RawAPIObject(amount_name, "ChangeAmount", api_objects)
    amount_raw_api_object.add_raw_parent("engine.util.attribute.AttributeAmount")
    amount_location = ForwardRef(pregen_converter_group, fallback_ref_in_modpack)
    amount_raw_api_object.set_location(amount_location)

    attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
    amount_raw_api_object.add_raw_member("type",
                                         attribute,
                                         "engine.util.attribute.AttributeAmount")
    amount_raw_api_object.add_raw_member("amount",
                                         1,
                                         "engine.util.attribute.AttributeAmount")

    pregen_converter_group.add_raw_api_object(amount_raw_api_object)
    pregen_nyan_objects.update({amount_name: amount_raw_api_object})

    # =================================================================================
    amount_forward_ref = ForwardRef(pregen_converter_group, amount_name)
    fallback_raw_api_object.add_raw_member("change_value",
                                           amount_forward_ref,
                                           DISCRETE_FLAC_EFFECT_PARENT)

    # Ignore protection
    fallback_raw_api_object.add_raw_member("ignore_protection",
                                           [],
                                           DISCRETE_FLAC_EFFECT_PARENT)

    pregen_converter_group.add_raw_api_object(fallback_raw_api_object)
    pregen_nyan_objects.update({fallback_ref_in_modpack: fallback_raw_api_object})


def _generate_fallback_resistances(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate fallback resistances for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    fallback_ref_in_modpack = (
        "resistance.discrete.flat_attribute_change.fallback.AoE2AttackFallback"
    )
    fallback_raw_api_object = RawAPIObject(fallback_ref_in_modpack,
                                           "AoE2AttackFallback",
                                           api_objects,
                                           FALLBACK_RESIST_LOCATION)
    fallback_raw_api_object.set_filename("fallback")
    fallback_raw_api_object.add_raw_parent(FALLBACK_RESIST_PARENT)

    # Type
    type_ref = "engine.util.attribute_change_type.type.Fallback"
    change_type = api_objects[type_ref]
    fallback_raw_api_object.add_raw_member("type",
                                           change_type,
                                           DISCRETE_FLAC_RESIST_PARENT)

    # Block value
    # =================================================================================
    amount_name = f"{fallback_ref_in_modpack}.BlockAmount"
    amount_raw_api_object = RawAPIObject(amount_name, "BlockAmount", api_objects)
    amount_raw_api_object.add_raw_parent("engine.util.attribute.AttributeAmount")
    amount_location = ForwardRef(pregen_converter_group, fallback_ref_in_modpack)
    amount_raw_api_object.set_location(amount_location)

    attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
    amount_raw_api_object.add_raw_member("type",
                                         attribute,
                                         "engine.util.attribute.AttributeAmount")
    amount_raw_api_object.add_raw_member("amount",
                                         0,
                                         "engine.util.attribute.AttributeAmount")

    pregen_converter_group.add_raw_api_object(amount_raw_api_object)
    pregen_nyan_objects.update({amount_name: amount_raw_api_object})

    # =================================================================================
    amount_forward_ref = ForwardRef(pregen_converter_group, amount_name)
    fallback_raw_api_object.add_raw_member("block_value",
                                           amount_forward_ref,
                                           DISCRETE_FLAC_RESIST_PARENT)

    pregen_converter_group.add_raw_api_object(fallback_raw_api_object)
    pregen_nyan_objects.update({fallback_ref_in_modpack: fallback_raw_api_object})


def _generate_construct_property(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the construct property for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    prop_ref_in_modpack = "resistance.property.types.BuildingConstruct"
    prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                       "BuildingConstruct",
                                       api_objects,
                                       "data/resistance/property/")
    prop_raw_api_object.set_filename("types")
    prop_raw_api_object.add_raw_parent("engine.resistance.property.type.Stacked")

    pregen_converter_group.add_raw_api_object(prop_raw_api_object)
    pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

    prop_raw_api_object.add_raw_member("stack_limit",
                                       MemberSpecialValue.NYAN_INF,
                                       "engine.resistance.property.type.Stacked")

    prop_raw_api_object.add_raw_member("distribution_type",
                                       api_objects["engine.util.distribution_type.type.Mean"],
                                       "engine.resistance.property.type.Stacked")

    # =======================================================================
    # Calculation type Construct
    # =======================================================================
    calc_parent = "engine.util.calculation_type.type.Hyperbolic"

    calc_ref_in_modpack = "util.calculation_type.construct_calculation.ConstructCalcType"
    calc_raw_api_object = RawAPIObject(calc_ref_in_modpack,
                                       "BuildingConstruct",
                                       api_objects)
    calc_location = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
    calc_raw_api_object.set_location(calc_location)
    calc_raw_api_object.add_raw_parent(calc_parent)

    pregen_converter_group.add_raw_api_object(calc_raw_api_object)
    pregen_nyan_objects.update({calc_ref_in_modpack: calc_raw_api_object})

    # Formula: (scale_factor / (count_effectors - shift_x)) + shift_y
    # AoE2: (3 / (vil_count + 2))

    # Shift x
    calc_raw_api_object.add_raw_member("shift_x",
                                       -2,
                                       calc_parent)

    # Shift y
    calc_raw_api_object.add_raw_member("shift_y",
                                       0,
                                       calc_parent)

    # Scale
    calc_raw_api_object.add_raw_member("scale_factor",
                                       3,
                                       calc_parent)

    calc_forward_ref = ForwardRef(pregen_converter_group, calc_ref_in_modpack)
    prop_raw_api_object.add_raw_member("calculation_type",
                                       calc_forward_ref,
                                       "engine.resistance.property.type.Stacked")


def _generate_repair_property(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the repair property for AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    prop_ref_in_modpack = "resistance.property.types.BuildingRepair"
    prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                       "BuildingRepair",
                                       api_objects,
                                       "data/resistance/property/")
    prop_raw_api_object.set_filename("types")
    prop_raw_api_object.add_raw_parent("engine.resistance.property.type.Stacked")

    pregen_converter_group.add_raw_api_object(prop_raw_api_object)
    pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

    prop_raw_api_object.add_raw_member("stack_limit",
                                       MemberSpecialValue.NYAN_INF,
                                       "engine.resistance.property.type.Stacked")

    prop_raw_api_object.add_raw_member("distribution_type",
                                       api_objects["engine.util.distribution_type.type.Mean"],
                                       "engine.resistance.property.type.Stacked")

    # =======================================================================
    # Calculation type Repair
    # =======================================================================
    calc_parent = "engine.util.calculation_type.type.Linear"

    calc_ref_in_modpack = "util.calculation_type.construct_calculation.BuildingRepair"
    calc_raw_api_object = RawAPIObject(calc_ref_in_modpack,
                                       "BuildingRepair",
                                       api_objects)
    calc_location = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
    calc_raw_api_object.set_location(calc_location)
    calc_raw_api_object.add_raw_parent(calc_parent)

    pregen_converter_group.add_raw_api_object(calc_raw_api_object)
    pregen_nyan_objects.update({calc_ref_in_modpack: calc_raw_api_object})

    # Formula: (scale_factor * (count_effectors - shift_x)) + shift_y
    # AoE2: (0.333334 * (vil_count + 2))

    # Shift x
    calc_raw_api_object.add_raw_member("shift_x",
                                       -2,
                                       calc_parent)

    # Shift y
    calc_raw_api_object.add_raw_member("shift_y",
                                       0,
                                       calc_parent)

    # Scale
    calc_raw_api_object.add_raw_member("scale_factor",
                                       1 / 3,
                                       calc_parent)

    calc_forward_ref = ForwardRef(pregen_converter_group, calc_ref_in_modpack)
    prop_raw_api_object.add_raw_member("calculation_type",
                                       calc_forward_ref,
                                       "engine.resistance.property.type.Stacked")

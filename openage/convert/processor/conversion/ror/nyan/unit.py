# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Convert unit lines to openage game entities.
"""
from .....entity_object.conversion.aoc.genie_unit import GenieVillagerGroup
from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from ...aoc.ability_subprocessor import AoCAbilitySubprocessor
from ...aoc.modifier_subprocessor import AoCModifierSubprocessor
from ..ability_subprocessor import RoRAbilitySubprocessor
from ..auxiliary_subprocessor import RoRAuxiliarySubprocessor
from .projectile import projectiles_from_line


def unit_line_to_game_entity(unit_line):
    """
    Creates raw API objects for a unit line.

    :param unit_line: Unit line that gets converted to a game entity.
    """
    current_unit = unit_line.get_head_unit()
    current_unit_id = unit_line.get_head_unit_id()

    dataset = unit_line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)
    class_lookup_dict = internal_name_lookups.get_class_lookups(dataset.game_version)

    # Start with the generic GameEntity
    game_entity_name = name_lookup_dict[current_unit_id][0]
    obj_location = f"data/game_entity/generic/{name_lookup_dict[current_unit_id][1]}/"
    raw_api_object = RawAPIObject(game_entity_name, game_entity_name,
                                  dataset.nyan_api_objects)
    raw_api_object.add_raw_parent("engine.util.game_entity.GameEntity")
    raw_api_object.set_location(obj_location)
    raw_api_object.set_filename(name_lookup_dict[current_unit_id][1])
    unit_line.add_raw_api_object(raw_api_object)

    # =======================================================================
    # Game Entity Types
    # =======================================================================
    # we give a unit two types
    #    - util.game_entity_type.types.Unit (if unit_type >= 70)
    #    - util.game_entity_type.types.<Class> (depending on the class)
    # =======================================================================
    # Create or use existing auxiliary types
    types_set = []
    unit_type = current_unit["unit_type"].value

    if unit_type >= 70:
        type_obj = dataset.pregen_nyan_objects["util.game_entity_type.types.Unit"].get_nyan_object(
        )
        types_set.append(type_obj)

    unit_class = current_unit["unit_class"].value
    class_name = class_lookup_dict[unit_class]
    class_obj_name = f"util.game_entity_type.types.{class_name}"
    type_obj = dataset.pregen_nyan_objects[class_obj_name].get_nyan_object()
    types_set.append(type_obj)

    raw_api_object.add_raw_member("types", types_set, "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Abilities
    # =======================================================================
    abilities_set = []

    abilities_set.append(AoCAbilitySubprocessor.activity_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.death_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.delete_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.despawn_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.idle_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.collision_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.live_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.los_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.move_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.named_ability(unit_line))
    abilities_set.append(RoRAbilitySubprocessor.resistance_ability(unit_line))
    abilities_set.extend(AoCAbilitySubprocessor.selectable_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.stop_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.terrain_requirement_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.turn_ability(unit_line))
    abilities_set.append(AoCAbilitySubprocessor.visibility_ability(unit_line))

    # Creation
    if len(unit_line.creates) > 0:
        abilities_set.append(AoCAbilitySubprocessor.create_ability(unit_line))

    # Config
    ability = AoCAbilitySubprocessor.use_contingent_ability(unit_line)
    if ability:
        abilities_set.append(ability)

    if unit_line.has_command(104):
        # Recharging attribute points (priests)
        abilities_set.extend(AoCAbilitySubprocessor.regenerate_attribute_ability(unit_line))

    # Applying effects and shooting projectiles
    if unit_line.is_projectile_shooter():
        abilities_set.append(RoRAbilitySubprocessor.shoot_projectile_ability(unit_line, 7))
        projectiles_from_line(unit_line)

    elif unit_line.is_melee() or unit_line.is_ranged():
        if unit_line.has_command(7):
            # Attack
            abilities_set.append(AoCAbilitySubprocessor.apply_discrete_effect_ability(
                unit_line,
                7,
                unit_line.is_ranged())
            )

        if unit_line.has_command(101):
            # Build
            abilities_set.append(AoCAbilitySubprocessor.apply_continuous_effect_ability(
                unit_line,
                101,
                unit_line.is_ranged())
            )

        if unit_line.has_command(104):
            # TODO: Success chance is not a resource in RoR
            # Convert
            abilities_set.append(RoRAbilitySubprocessor.apply_discrete_effect_ability(
                unit_line,
                104,
                unit_line.is_ranged())
            )

        if unit_line.has_command(105):
            # Heal
            abilities_set.append(AoCAbilitySubprocessor.apply_continuous_effect_ability(
                unit_line,
                105,
                unit_line.is_ranged())
            )

        if unit_line.has_command(106):
            # Repair
            abilities_set.append(AoCAbilitySubprocessor.apply_continuous_effect_ability(
                unit_line,
                106,
                unit_line.is_ranged())
            )

    # Formation/Stance
    if not isinstance(unit_line, GenieVillagerGroup):
        abilities_set.append(RoRAbilitySubprocessor.game_entity_stance_ability(unit_line))

    # Storage abilities
    if unit_line.is_garrison():
        abilities_set.append(AoCAbilitySubprocessor.storage_ability(unit_line))
        abilities_set.append(AoCAbilitySubprocessor.remove_storage_ability(unit_line))

    if len(unit_line.garrison_locations) > 0:
        ability = AoCAbilitySubprocessor.enter_container_ability(unit_line)
        if ability:
            abilities_set.append(ability)

        ability = AoCAbilitySubprocessor.exit_container_ability(unit_line)
        if ability:
            abilities_set.append(ability)

    # Resource abilities
    if unit_line.is_gatherer():
        abilities_set.append(AoCAbilitySubprocessor.drop_resources_ability(unit_line))
        abilities_set.extend(AoCAbilitySubprocessor.gather_ability(unit_line))

    # Resource storage
    if unit_line.is_gatherer() or unit_line.has_command(111):
        abilities_set.append(AoCAbilitySubprocessor.resource_storage_ability(unit_line))

    if unit_line.is_harvestable():
        abilities_set.append(AoCAbilitySubprocessor.harvestable_ability(unit_line))

    # Trade abilities
    if unit_line.has_command(111):
        abilities_set.append(AoCAbilitySubprocessor.trade_ability(unit_line))

    raw_api_object.add_raw_member("abilities", abilities_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Modifiers
    # =======================================================================
    modifiers_set = []

    if unit_line.is_gatherer():
        modifiers_set.extend(AoCModifierSubprocessor.gather_rate_modifier(unit_line))

    # TODO: Other modifiers?

    raw_api_object.add_raw_member("modifiers", modifiers_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # TODO: Variants
    # =======================================================================
    variants_set = []

    raw_api_object.add_raw_member("variants", variants_set,
                                  "engine.util.game_entity.GameEntity")

    # =======================================================================
    # Misc (Objects that are not used by the unit line itself, but use its values)
    # =======================================================================
    if unit_line.is_creatable():
        RoRAuxiliarySubprocessor.get_creatable_game_entity(unit_line)

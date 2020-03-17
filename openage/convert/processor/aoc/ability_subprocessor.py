# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Derives and adds abilities for to unit lines. Subroutine of the
nyan subprocessor.
"""
from ...dataformat.converter_object import RawAPIObject
from ...dataformat.aoc.expected_pointer import ExpectedPointer
from ...dataformat.aoc.internal_nyan_names import UNIT_LINE_LOOKUPS, BUILDING_LINE_LOOKUPS
from ...dataformat.aoc.genie_unit import GenieVillagerGroup
from ...dataformat.aoc.combined_sprite import CombinedSprite
from openage.nyan.nyan_structs import MemberSpecialValue
from openage.convert.dataformat.aoc.genie_unit import GenieBuildingLineGroup
from plainbox.impl.session import storage
from openage.convert.dataformat.aoc.internal_nyan_names import TECH_GROUP_LOOKUPS


class AoCAbilitySubprocessor:

    @staticmethod
    def create_ability(line):
        """
        Adds the Create ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS
            creatable_lookup_dict = UNIT_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS
            creatable_lookup_dict = BUILDING_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        obj_name = "%s.Create" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Create", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Create")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        creatables_set = []

        for creatable in line.creates:
            if creatable.is_unique():
                # Skip this because unique units are handled by civs
                continue

            # CreatableGameEntity objects are created for each unit/building
            # line individually to avoid duplicates. We just point to the
            # raw API objects here.
            creatable_id = creatable.get_head_unit_id()
            creatable_name = creatable_lookup_dict[creatable_id][0]

            raw_api_object_ref = "%s.CreatableGameEntity" % creatable_name
            creatable_expected_pointer = ExpectedPointer(creatable,
                                                         raw_api_object_ref)
            creatables_set.append(creatable_expected_pointer)

        ability_raw_api_object.add_raw_member("creatables", creatables_set,
                                              "engine.ability.type.Create")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def hitbox_ability(line):
        """
        Adds the Hitbox ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Hitbox" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Hitbox", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Hitbox")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Hitbox object
        hitbox_name = "%s.Hitbox.%sHitbox" % (game_entity_name, game_entity_name)
        hitbox_raw_api_object = RawAPIObject(hitbox_name,
                                             "%sHitbox" % (game_entity_name),
                                             dataset.nyan_api_objects)
        hitbox_raw_api_object.add_raw_parent("engine.aux.hitbox.Hitbox")
        hitbox_location = ExpectedPointer(line, obj_name)
        hitbox_raw_api_object.set_location(hitbox_location)

        radius_x = current_unit.get_member("radius_x").get_value()
        radius_y = current_unit.get_member("radius_y").get_value()
        radius_z = current_unit.get_member("radius_z").get_value()

        hitbox_raw_api_object.add_raw_member("radius_x",
                                             radius_x,
                                             "engine.aux.hitbox.Hitbox")
        hitbox_raw_api_object.add_raw_member("radius_y",
                                             radius_y,
                                             "engine.aux.hitbox.Hitbox")
        hitbox_raw_api_object.add_raw_member("radius_z",
                                             radius_z,
                                             "engine.aux.hitbox.Hitbox")

        hitbox_expected_pointer = ExpectedPointer(line, hitbox_name)
        ability_raw_api_object.add_raw_member("hitbox",
                                              hitbox_expected_pointer,
                                              "engine.ability.type.Hitbox")

        line.add_raw_api_object(hitbox_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def idle_ability(line):
        """
        Adds the Idle ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Idle" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Idle", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Idle")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        ability_animation_id = current_unit.get_member("idle_graphic0").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Idle.IdleAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "IdleAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, "%s.Idle" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "idle_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, obj_name)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def live_ability(line):
        """
        Adds the Live ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Live" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Live", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Live")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        attributes_set = []

        obj_name = "%s.Live.Health" % (game_entity_name)
        health_raw_api_object = RawAPIObject(obj_name, "Health", dataset.nyan_api_objects)
        health_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeSetting")
        health_location = ExpectedPointer(line, "%s.Live" % (game_entity_name))
        health_raw_api_object.set_location(health_location)

        attribute_value = dataset.pregen_nyan_objects["aux.attribute.types.Health"].get_nyan_object()
        health_raw_api_object.add_raw_member("attribute", attribute_value,
                                             "engine.aux.attribute.AttributeSetting")

        # Lowest HP can go
        health_raw_api_object.add_raw_member("min_value", -1,
                                             "engine.aux.attribute.AttributeSetting")

        # Max HP and starting HP
        max_hp_value = current_unit.get_member("hit_points").get_value()
        health_raw_api_object.add_raw_member("max_value", max_hp_value,
                                             "engine.aux.attribute.AttributeSetting")
        health_raw_api_object.add_raw_member("starting_value", max_hp_value,
                                             "engine.aux.attribute.AttributeSetting")

        health_expected_pointer = ExpectedPointer(line, health_raw_api_object.get_id())
        attributes_set.append(health_expected_pointer)
        ability_raw_api_object.add_raw_member("attributes", attributes_set,
                                              "engine.ability.type.Live")

        line.add_raw_api_object(health_raw_api_object)
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def los_ability(line):
        """
        Adds the LineOfSight ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.LineOfSight" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "LineOfSight", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.LineOfSight")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Line of sight
        line_of_sight = current_unit.get_member("line_of_sight").get_value()
        ability_raw_api_object.add_raw_member("range", line_of_sight,
                                              "engine.ability.type.LineOfSight")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def move_ability(line):
        """
        Adds the Move ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Move" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Move", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Move")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Animation
        ability_animation_id = current_unit.get_member("move_graphics").get_value()

        if ability_animation_id > -1:
            # Make the ability animated
            ability_raw_api_object.add_raw_parent("engine.ability.specialization.AnimatedAbility")

            animations_set = []

            # Create animation object
            obj_name = "%s.Move.MoveAnimation" % (game_entity_name)
            animation_raw_api_object = RawAPIObject(obj_name, "MoveAnimation",
                                                    dataset.nyan_api_objects)
            animation_raw_api_object.add_raw_parent("engine.aux.graphics.Animation")
            animation_location = ExpectedPointer(line, "%s.Move" % (game_entity_name))
            animation_raw_api_object.set_location(animation_location)

            ability_sprite = CombinedSprite(ability_animation_id,
                                            "move_%s" % (name_lookup_dict[current_unit_id][1]),
                                            dataset)
            dataset.combined_sprites.update({ability_sprite.get_id(): ability_sprite})
            ability_sprite.add_reference(animation_raw_api_object)

            animation_raw_api_object.add_raw_member("sprite", ability_sprite,
                                                    "engine.aux.graphics.Animation")

            animation_expected_pointer = ExpectedPointer(line, obj_name)
            animations_set.append(animation_expected_pointer)

            ability_raw_api_object.add_raw_member("animations", animations_set,
                                                  "engine.ability.specialization.AnimatedAbility")

            line.add_raw_api_object(animation_raw_api_object)

        # Speed
        speed = current_unit.get_member("speed").get_value()
        ability_raw_api_object.add_raw_member("speed", speed, "engine.ability.type.Move")

        # Standard move modes
        move_modes = [dataset.nyan_api_objects["engine.aux.move_mode.type.AttackMove"],
                      dataset.nyan_api_objects["engine.aux.move_mode.type.Normal"],
                      dataset.nyan_api_objects["engine.aux.move_mode.type.Patrol"]]
        # Follow
        obj_name = "%s.Move.Follow" % (game_entity_name)
        follow_raw_api_object = RawAPIObject(obj_name, "Follow", dataset.nyan_api_objects)
        follow_raw_api_object.add_raw_parent("engine.aux.move_mode.type.Follow")
        follow_location = ExpectedPointer(line, "%s.Move" % (game_entity_name))
        follow_raw_api_object.set_location(follow_location)

        follow_range = current_unit.get_member("line_of_sight").get_value() - 1
        follow_raw_api_object.add_raw_member("range", follow_range,
                                             "engine.aux.move_mode.type.Follow")

        line.add_raw_api_object(follow_raw_api_object)
        follow_expected_pointer = ExpectedPointer(line, follow_raw_api_object.get_id())
        move_modes.append(follow_expected_pointer)

        ability_raw_api_object.add_raw_member("modes", move_modes, "engine.ability.type.Move")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def named_ability(line):
        """
        Adds the Named ability to a line.

        TODO: Lookup names from language.dll

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Named" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Named", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Named")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Name
        name_ref = "%s.Named.%sName" % (game_entity_name, game_entity_name)
        name_raw_api_object = RawAPIObject(name_ref,
                                           "%sName"  % (game_entity_name),
                                           dataset.nyan_api_objects)
        name_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedString")
        name_location = ExpectedPointer(line, obj_name)
        name_raw_api_object.set_location(name_location)

        name_raw_api_object.add_raw_member("translations",
                                           [],
                                           "engine.aux.translated.type.TranslatedString")

        name_expected_pointer = ExpectedPointer(line, name_ref)
        ability_raw_api_object.add_raw_member("name", name_expected_pointer, "engine.ability.type.Named")
        line.add_raw_api_object(name_raw_api_object)

        # Description
        description_ref = "%s.Named.%sDescription" % (game_entity_name, game_entity_name)
        description_raw_api_object = RawAPIObject(description_ref,
                                                  "%sDescription"  % (game_entity_name),
                                                  dataset.nyan_api_objects)
        description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        description_location = ExpectedPointer(line, obj_name)
        description_raw_api_object.set_location(description_location)

        description_raw_api_object.add_raw_member("translations",
                                                  [],
                                                  "engine.aux.translated.type.TranslatedMarkupFile")

        description_expected_pointer = ExpectedPointer(line, description_ref)
        ability_raw_api_object.add_raw_member("description",
                                              description_expected_pointer,
                                              "engine.ability.type.Named")
        line.add_raw_api_object(description_raw_api_object)

        # Long description
        long_description_ref = "%s.Named.%sLongDescription" % (game_entity_name, game_entity_name)
        long_description_raw_api_object = RawAPIObject(long_description_ref,
                                                       "%sLongDescription"  % (game_entity_name),
                                                       dataset.nyan_api_objects)
        long_description_raw_api_object.add_raw_parent("engine.aux.translated.type.TranslatedMarkupFile")
        long_description_location = ExpectedPointer(line, obj_name)
        long_description_raw_api_object.set_location(long_description_location)

        long_description_raw_api_object.add_raw_member("translations",
                                                       [],
                                                       "engine.aux.translated.type.TranslatedMarkupFile")

        long_description_expected_pointer = ExpectedPointer(line, long_description_ref)
        ability_raw_api_object.add_raw_member("long_description",
                                              long_description_expected_pointer,
                                              "engine.ability.type.Named")
        line.add_raw_api_object(long_description_raw_api_object)

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def provide_contingent_ability(line):
        """
        Adds the ProvideContingent ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.ProvideContingent" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "ProvideContingent", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.ProvideContingent")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Also stores the pop space
        resource_storage = current_unit.get_member("resource_storage").get_value()

        contingents = []
        for storage in resource_storage:
            type_id = storage.get_value()["type"].get_value()

            if type_id == 4:
                resource = dataset.pregen_nyan_objects["aux.resource.types.PopulationSpace"].get_nyan_object()
                resource_name = "PopSpace"

            else:
                continue

            amount = storage.get_value()["amount"].get_value()

            contingent_amount_name = "%s.ProvideContingent.%s" % (game_entity_name, resource_name)
            contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                             dataset.nyan_api_objects)
            contingent_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            ability_expected_pointer = ExpectedPointer(line, obj_name)
            contingent_amount.set_location(ability_expected_pointer)

            contingent_amount.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")
            contingent_amount.add_raw_member("amount",
                                             amount,
                                             "engine.aux.resource.ResourceAmount")

            line.add_raw_api_object(contingent_amount)
            contingent_amount_expected_pointer = ExpectedPointer(line,
                                                                 contingent_amount_name)
            contingents.append(contingent_amount_expected_pointer)

        if not contingents:
            # Do not create the ability if its values are empty
            return None

        ability_raw_api_object.add_raw_member("amount",
                                              contingents,
                                              "engine.ability.type.ProvideContingent")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def research_ability(line):
        """
        Adds the Research ability to a line.

        :param line: Unit/Building line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]
        obj_name = "%s.Research" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Research", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Research")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        researchables_set = []

        for researchable in line.researches:
            if researchable.is_unique():
                # Skip this because unique techs are handled by civs
                continue

            # ResearchableTech objects are created for each unit/building
            # line individually to avoid duplicates. We just point to the
            # raw API objects here.
            researchable_id = researchable.get_id()
            researchable_name = TECH_GROUP_LOOKUPS[researchable_id][0]

            raw_api_object_ref = "%s.ResearchableTech" % researchable_name
            researchable_expected_pointer = ExpectedPointer(researchable,
                                                            raw_api_object_ref)
            researchables_set.append(researchable_expected_pointer)

        ability_raw_api_object.add_raw_member("researchables", researchables_set,
                                              "engine.ability.type.Research")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def stop_ability(line):
        """
        Adds the Stop ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Stop" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Stop", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Stop")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def turn_ability(line):
        """
        Adds the Turn ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Turn" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Turn", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Turn")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Speed
        turn_speed_unmodified = current_unit.get_member("turn_speed").get_value()

        # Default case: Instant turning
        turn_speed = MemberSpecialValue.NYAN_INF

        # Ships/Trebuchets turn slower
        if turn_speed_unmodified >= 0:
            # TODO: Calculate this
            pass

        ability_raw_api_object.add_raw_member("turn_speed", turn_speed, "engine.ability.type.Turn")

        # Diplomacy settings
        ability_raw_api_object.add_raw_parent("engine.ability.specialization.DiplomaticAbility")
        diplomatic_stances = [dataset.nyan_api_objects["engine.aux.diplomatic_stance.type.Self"]]
        ability_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                              "engine.ability.specialization.DiplomaticAbility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def use_contingent_ability(line):
        """
        Adds the UseContingent ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        if isinstance(line, GenieVillagerGroup):
            # TODO: Requires special treatment?
            current_unit = line.variants[0].line[0]

        else:
            current_unit = line.line[0]

        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.UseContingent" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "UseContingent", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.UseContingent")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Also stores the pop space
        resource_storage = current_unit.get_member("resource_storage").get_value()

        contingents = []
        for storage in resource_storage:
            type_id = storage.get_value()["type"].get_value()

            if type_id == 11:
                resource = dataset.pregen_nyan_objects["aux.resource.types.PopulationSpace"].get_nyan_object()
                resource_name = "PopSpace"

            else:
                continue

            amount = storage.get_value()["amount"].get_value()

            contingent_amount_name = "%s.UseContingent.%s" % (game_entity_name, resource_name)
            contingent_amount = RawAPIObject(contingent_amount_name, resource_name,
                                             dataset.nyan_api_objects)
            contingent_amount.add_raw_parent("engine.aux.resource.ResourceAmount")
            ability_expected_pointer = ExpectedPointer(line, obj_name)
            contingent_amount.set_location(ability_expected_pointer)

            contingent_amount.add_raw_member("type",
                                             resource,
                                             "engine.aux.resource.ResourceAmount")
            contingent_amount.add_raw_member("amount",
                                             amount,
                                             "engine.aux.resource.ResourceAmount")

            line.add_raw_api_object(contingent_amount)
            contingent_amount_expected_pointer = ExpectedPointer(line,
                                                                 contingent_amount_name)
            contingents.append(contingent_amount_expected_pointer)

        if not contingents:
            # Do not create the ability if its values are empty
            return None

        ability_raw_api_object.add_raw_member("amount",
                                              contingents,
                                              "engine.ability.type.UseContingent")
        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

    @staticmethod
    def visibility_ability(line):
        """
        Adds the Visibility ability to a line.

        :param line: Unit line that gets the ability.
        :type line: ...dataformat.converter_object.ConverterObjectGroup
        :returns: The expected pointer for the ability.
        :rtype: ...dataformat.expected_pointer.ExpectedPointer
        """
        current_unit_id = line.get_head_unit_id()
        dataset = line.data

        if isinstance(line, GenieBuildingLineGroup):
            name_lookup_dict = BUILDING_LINE_LOOKUPS

        else:
            name_lookup_dict = UNIT_LINE_LOOKUPS

        game_entity_name = name_lookup_dict[current_unit_id][0]

        obj_name = "%s.Visibility" % (game_entity_name)
        ability_raw_api_object = RawAPIObject(obj_name, "Visibility", dataset.nyan_api_objects)
        ability_raw_api_object.add_raw_parent("engine.ability.type.Visibility")
        ability_location = ExpectedPointer(line, game_entity_name)
        ability_raw_api_object.set_location(ability_location)

        # Units are not visible in fog
        ability_raw_api_object.add_raw_member("visible_in_fog", False,
                                              "engine.ability.type.Visibility")

        line.add_raw_api_object(ability_raw_api_object)

        ability_expected_pointer = ExpectedPointer(line, ability_raw_api_object.get_id())

        return ability_expected_pointer

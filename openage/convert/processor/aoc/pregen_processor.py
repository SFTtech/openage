# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.

"""
from openage.convert.dataformat.converter_object import RawAPIObject,\
    ConverterObjectGroup
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.nyan.nyan_structs import MemberSpecialValue
from openage.convert.dataformat.aoc.internal_nyan_names import CLASS_ID_LOOKUPS,\
    ARMOR_CLASS_LOOKUPS, TERRAIN_TYPE_LOOKUPS, BUILDING_LINE_LOOKUPS,\
    UNIT_LINE_LOOKUPS
from openage.convert.dataformat.aoc.genie_unit import GenieUnitLineGroup


class AoCPregenSubprocessor:

    @classmethod
    def generate(cls, gamedata):
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        cls._generate_attributes(gamedata, pregen_converter_group)
        cls._generate_diplomatic_stances(gamedata, pregen_converter_group)
        cls._generate_entity_types(gamedata, pregen_converter_group)
        cls._generate_effect_types(gamedata, pregen_converter_group)
        cls._generate_misc_effect_objects(gamedata, pregen_converter_group)
        cls._generate_terrain_types(gamedata, pregen_converter_group)
        cls._generate_resources(gamedata, pregen_converter_group)
        cls._generate_death_condition(gamedata, pregen_converter_group)

        pregen_nyan_objects = gamedata.pregen_nyan_objects
        # Create nyan objects from the raw API objects
        for pregen_object in pregen_nyan_objects.values():
            pregen_object.create_nyan_object()

        # This has to be separate because of possible object interdependencies
        for pregen_object in pregen_nyan_objects.values():
            pregen_object.create_nyan_members()

            if not pregen_object.is_ready():
                raise Exception("%s: Pregenerated object is not ready for export."
                                "Member or object not initialized." % (pregen_object))

    @staticmethod
    def _generate_attributes(full_data_set, pregen_converter_group):
        """
        Generate Attribute objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # TODO: Fill translations
        # =======================================================================
        attribute_parent = "engine.aux.attribute.Attribute"
        attributes_location = "data/aux/attribute/"

        # =======================================================================
        # HP
        # =======================================================================
        health_ref_in_modpack = "aux.attribute.types.Health"
        health_raw_api_object = RawAPIObject(health_ref_in_modpack,
                                             "Health", api_objects,
                                             attributes_location)
        health_raw_api_object.set_filename("types")
        health_raw_api_object.add_raw_parent(attribute_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                "aux.attribute.types.Health.HealthName")
        health_raw_api_object.add_raw_member("name", name_expected_pointer,
                                             attribute_parent)
        abbrv_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                 "aux.attribute.types.Health.HealthAbbreviation")
        health_raw_api_object.add_raw_member("abbreviation", abbrv_expected_pointer,
                                             attribute_parent)

        pregen_converter_group.add_raw_api_object(health_raw_api_object)
        pregen_nyan_objects.update({health_ref_in_modpack: health_raw_api_object})

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        health_name_ref_in_modpack = "aux.attribute.types.Health.HealthName"
        health_name_value = RawAPIObject(health_name_ref_in_modpack, "HealthName",
                                         api_objects, attributes_location)
        health_name_value.set_filename("types")
        health_name_value.add_raw_parent(name_value_parent)
        health_name_value.add_raw_member("translations", [], name_value_parent)

        pregen_converter_group.add_raw_api_object(health_name_value)
        pregen_nyan_objects.update({health_name_ref_in_modpack: health_name_value})

        abbrv_value_parent = "engine.aux.translated.type.TranslatedString"
        health_abbrv_ref_in_modpack = "aux.attribute.types.Health.HealthAbbreviation"
        health_abbrv_value = RawAPIObject(health_abbrv_ref_in_modpack, "HealthAbbreviation",
                                          api_objects, attributes_location)
        health_abbrv_value.set_filename("types")
        health_abbrv_value.add_raw_parent(abbrv_value_parent)
        health_abbrv_value.add_raw_member("translations", [], abbrv_value_parent)

        pregen_converter_group.add_raw_api_object(health_abbrv_value)
        pregen_nyan_objects.update({health_abbrv_ref_in_modpack: health_abbrv_value})

        # =======================================================================
        # Faith
        # =======================================================================
        faith_ref_in_modpack = "aux.attribute.types.Faith"
        faith_raw_api_object = RawAPIObject(faith_ref_in_modpack,
                                            "Faith", api_objects,
                                            attributes_location)
        faith_raw_api_object.set_filename("types")
        faith_raw_api_object.add_raw_parent(attribute_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                "aux.attribute.types.Faith.FaithName")
        faith_raw_api_object.add_raw_member("name", name_expected_pointer,
                                            attribute_parent)
        abbrv_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                 "aux.attribute.types.Faith.FaithAbbreviation")
        faith_raw_api_object.add_raw_member("abbreviation", abbrv_expected_pointer,
                                            attribute_parent)

        pregen_converter_group.add_raw_api_object(faith_raw_api_object)
        pregen_nyan_objects.update({faith_ref_in_modpack: faith_raw_api_object})

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        faith_name_ref_in_modpack = "aux.attribute.types.Faith.FaithName"
        faith_name_value = RawAPIObject(faith_name_ref_in_modpack, "FaithName",
                                        api_objects, attributes_location)
        faith_name_value.set_filename("types")
        faith_name_value.add_raw_parent(name_value_parent)
        faith_name_value.add_raw_member("translations", [], name_value_parent)

        pregen_converter_group.add_raw_api_object(faith_name_value)
        pregen_nyan_objects.update({faith_name_ref_in_modpack: faith_name_value})

        abbrv_value_parent = "engine.aux.translated.type.TranslatedString"
        faith_abbrv_ref_in_modpack = "aux.attribute.types.Faith.FaithAbbreviation"
        faith_abbrv_value = RawAPIObject(faith_abbrv_ref_in_modpack, "FaithAbbreviation",
                                         api_objects, attributes_location)
        faith_abbrv_value.set_filename("types")
        faith_abbrv_value.add_raw_parent(abbrv_value_parent)
        faith_abbrv_value.add_raw_member("translations", [], abbrv_value_parent)

        pregen_converter_group.add_raw_api_object(faith_abbrv_value)
        pregen_nyan_objects.update({faith_abbrv_ref_in_modpack: faith_abbrv_value})

    @staticmethod
    def _generate_diplomatic_stances(full_data_set, pregen_converter_group):
        """
        Generate DiplomaticStance objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        stance_parent = "engine.aux.diplomatic_stance.DiplomaticStance"
        stance_location = "data/aux/diplomatic_stance/"

        # =======================================================================
        # Enemy
        # =======================================================================
        enemy_ref_in_modpack = "aux.diplomatic_stance.types.Enemy"
        enemy_raw_api_object = RawAPIObject(enemy_ref_in_modpack,
                                            "Enemy", api_objects,
                                            stance_location)
        enemy_raw_api_object.set_filename("types")
        enemy_raw_api_object.add_raw_parent(stance_parent)

        pregen_converter_group.add_raw_api_object(enemy_raw_api_object)
        pregen_nyan_objects.update({enemy_ref_in_modpack: enemy_raw_api_object})

        # =======================================================================
        # Neutral
        # =======================================================================
        neutral_ref_in_modpack = "aux.diplomatic_stance.types.Neutral"
        neutral_raw_api_object = RawAPIObject(neutral_ref_in_modpack,
                                              "Neutral", api_objects,
                                              stance_location)
        neutral_raw_api_object.set_filename("types")
        neutral_raw_api_object.add_raw_parent(stance_parent)

        pregen_converter_group.add_raw_api_object(neutral_raw_api_object)
        pregen_nyan_objects.update({neutral_ref_in_modpack: neutral_raw_api_object})

        # =======================================================================
        # Friendly
        # =======================================================================
        friendly_ref_in_modpack = "aux.diplomatic_stance.types.Friendly"
        friendly_raw_api_object = RawAPIObject(friendly_ref_in_modpack,
                                               "Friendly", api_objects,
                                               stance_location)
        friendly_raw_api_object.set_filename("types")
        friendly_raw_api_object.add_raw_parent(stance_parent)

        pregen_converter_group.add_raw_api_object(friendly_raw_api_object)
        pregen_nyan_objects.update({friendly_ref_in_modpack: friendly_raw_api_object})

        # =======================================================================
        # Gaia
        # =======================================================================
        gaia_ref_in_modpack = "aux.diplomatic_stance.types.Gaia"
        gaia_raw_api_object = RawAPIObject(gaia_ref_in_modpack,
                                           "Gaia", api_objects,
                                           stance_location)
        gaia_raw_api_object.set_filename("types")
        gaia_raw_api_object.add_raw_parent(stance_parent)

        pregen_converter_group.add_raw_api_object(gaia_raw_api_object)
        pregen_nyan_objects.update({gaia_ref_in_modpack: gaia_raw_api_object})

    @staticmethod
    def _generate_entity_types(full_data_set, pregen_converter_group):
        """
        Generate GameEntityType objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        type_parent = "engine.aux.game_entity_type.GameEntityType"
        types_location = "data/aux/game_entity_type/"

        # =======================================================================
        # Ambient
        # =======================================================================
        ambient_ref_in_modpack = "aux.game_entity_type.types.Ambient"
        ambient_raw_api_object = RawAPIObject(ambient_ref_in_modpack,
                                              "Ambient", api_objects,
                                              types_location)
        ambient_raw_api_object.set_filename("types")
        ambient_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(ambient_raw_api_object)
        pregen_nyan_objects.update({ambient_ref_in_modpack: ambient_raw_api_object})

        # =======================================================================
        # Building
        # =======================================================================
        building_ref_in_modpack = "aux.game_entity_type.types.Building"
        building_raw_api_object = RawAPIObject(building_ref_in_modpack,
                                               "Building", api_objects,
                                               types_location)
        building_raw_api_object.set_filename("types")
        building_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(building_raw_api_object)
        pregen_nyan_objects.update({building_ref_in_modpack: building_raw_api_object})

        # =======================================================================
        # Item
        # =======================================================================
        item_ref_in_modpack = "aux.game_entity_type.types.Item"
        item_raw_api_object = RawAPIObject(item_ref_in_modpack,
                                           "Item", api_objects,
                                           types_location)
        item_raw_api_object.set_filename("types")
        item_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(item_raw_api_object)
        pregen_nyan_objects.update({item_ref_in_modpack: item_raw_api_object})

        # =======================================================================
        # Projectile
        # =======================================================================
        projectile_ref_in_modpack = "aux.game_entity_type.types.Projectile"
        projectile_raw_api_object = RawAPIObject(projectile_ref_in_modpack,
                                                 "Projectile", api_objects,
                                                 types_location)
        projectile_raw_api_object.set_filename("types")
        projectile_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(projectile_raw_api_object)
        pregen_nyan_objects.update({projectile_ref_in_modpack: projectile_raw_api_object})

        # =======================================================================
        # Unit
        # =======================================================================
        unit_ref_in_modpack = "aux.game_entity_type.types.Unit"
        unit_raw_api_object = RawAPIObject(unit_ref_in_modpack,
                                           "Unit", api_objects,
                                           types_location)
        unit_raw_api_object.set_filename("types")
        unit_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(unit_raw_api_object)
        pregen_nyan_objects.update({unit_ref_in_modpack: unit_raw_api_object})

        # =======================================================================
        # DropSite
        # =======================================================================
        drop_site_ref_in_modpack = "aux.game_entity_type.types.DropSite"
        drop_site_raw_api_object = RawAPIObject(drop_site_ref_in_modpack,
                                                "DropSite", api_objects,
                                                types_location)
        drop_site_raw_api_object.set_filename("types")
        drop_site_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(drop_site_raw_api_object)
        pregen_nyan_objects.update({drop_site_ref_in_modpack: drop_site_raw_api_object})

        # =======================================================================
        # Others (generated from class ID)
        # =======================================================================
        converter_groups = []
        converter_groups.extend(full_data_set.unit_lines.values())
        converter_groups.extend(full_data_set.building_lines.values())
        converter_groups.extend(full_data_set.ambient_groups.values())
        converter_groups.extend(full_data_set.variant_groups.values())

        for unit_line in converter_groups:
            unit_class = unit_line.get_class_id()
            class_name = CLASS_ID_LOOKUPS[unit_class]
            class_obj_name = "aux.game_entity_type.types.%s" % (class_name)

            new_game_entity_type = RawAPIObject(class_obj_name, class_name,
                                                full_data_set.nyan_api_objects,
                                                types_location)
            new_game_entity_type.set_filename("types")
            new_game_entity_type.add_raw_parent("engine.aux.game_entity_type.GameEntityType")
            new_game_entity_type.create_nyan_object()
            full_data_set.pregen_nyan_objects.update({class_obj_name: new_game_entity_type})

    @staticmethod
    def _generate_effect_types(full_data_set, pregen_converter_group):
        """
        Generate types for effects and resistances.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # AttributeChangeType
        # =======================================================================
        type_parent = "engine.aux.attribute_change_type.AttributeChangeType"
        types_location = "data/aux/attribute_change_type/"

        for type_name in ARMOR_CLASS_LOOKUPS.values():
            type_ref_in_modpack = "aux.attribute_change_type.types.%s" % (type_name)
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               type_name, api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        # =======================================================================
        # Heal
        # =======================================================================
        type_ref_in_modpack = "aux.attribute_change_type.types.Heal"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           "Heal", api_objects,
                                           types_location)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        # =======================================================================
        # Repair (one for each repairable entity)
        # =======================================================================
        repairable_lines = []
        repairable_lines.extend(full_data_set.building_lines.values())
        for unit_line in full_data_set.unit_lines.values():
            if unit_line.get_class_id() in (2, 13, 20, 21, 22, 55):
                repairable_lines.append(unit_line)

        for repairable_line in repairable_lines:
            if isinstance(repairable_line, GenieUnitLineGroup):
                game_entity_name = UNIT_LINE_LOOKUPS[repairable_line.get_head_unit_id()][0]

            else:
                game_entity_name = BUILDING_LINE_LOOKUPS[repairable_line.get_head_unit_id()][0]

            type_ref_in_modpack = "aux.attribute_change_type.types.%sRepair" % (game_entity_name)
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               "%sRepair" % (game_entity_name),
                                               api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        # =======================================================================
        # Construct (two for each constructable entity)
        # =======================================================================
        constructable_lines = []
        constructable_lines.extend(full_data_set.building_lines.values())

        for constructable_line in constructable_lines:
            if isinstance(constructable_line, GenieUnitLineGroup):
                game_entity_name = UNIT_LINE_LOOKUPS[constructable_line.get_head_unit_id()][0]

            else:
                game_entity_name = BUILDING_LINE_LOOKUPS[constructable_line.get_head_unit_id()][0]

            type_ref_in_modpack = "aux.attribute_change_type.types.%sConstruct" % (game_entity_name)
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               "%sConstruct" % (game_entity_name),
                                               api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        type_parent = "engine.aux.progress_type.type.Construct"
        types_location = "data/aux/construct_type/"

        for constructable_line in constructable_lines:
            if isinstance(constructable_line, GenieUnitLineGroup):
                game_entity_name = UNIT_LINE_LOOKUPS[constructable_line.get_head_unit_id()][0]

            else:
                game_entity_name = BUILDING_LINE_LOOKUPS[constructable_line.get_head_unit_id()][0]

            type_ref_in_modpack = "aux.construct_type.types.%sConstruct" % (game_entity_name)
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               "%sConstruct" % (game_entity_name),
                                               api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        # =======================================================================
        # ConvertType: UnitConvert
        # =======================================================================
        type_parent = "engine.aux.convert_type.ConvertType"
        types_location = "data/aux/convert_type/"

        type_ref_in_modpack = "aux.convert_type.types.UnitConvert"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           "UnitConvert", api_objects,
                                           types_location)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        # =======================================================================
        # ConvertType: BuildingConvert
        # =======================================================================
        type_parent = "engine.aux.convert_type.ConvertType"
        types_location = "data/aux/convert_type/"

        type_ref_in_modpack = "aux.convert_type.types.BuildingConvert"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           "BuildingConvert", api_objects,
                                           types_location)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    @staticmethod
    def _generate_misc_effect_objects(full_data_set, pregen_converter_group):
        """
        Generate fallback types and other standard objects for effects and resistances.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # Min change value (lower cealing for attack effects)
        # =======================================================================
        min_change_parent = "engine.aux.attribute.AttributeAmount"
        min_change_location = "data/effect/discrete/flat_attribute_change/"

        change_ref_in_modpack = "effect.discrete.flat_attribute_change.min_damage.AoE2MinChangeAmount"
        change_raw_api_object = RawAPIObject(change_ref_in_modpack,
                                             "AoE2MinChangeAmount",
                                             api_objects,
                                             min_change_location)
        change_raw_api_object.set_filename("min_damage")
        change_raw_api_object.add_raw_parent(min_change_parent)

        attribute = ExpectedPointer(pregen_converter_group, "aux.attribute.types.Health")
        change_raw_api_object.add_raw_member("type",
                                             attribute,
                                             min_change_parent)
        change_raw_api_object.add_raw_member("amount",
                                             0,
                                             min_change_parent)

        pregen_converter_group.add_raw_api_object(change_raw_api_object)
        pregen_nyan_objects.update({change_ref_in_modpack: change_raw_api_object})

        # =======================================================================
        # Min change value (lower cealing for heal effects)
        # =======================================================================
        min_change_parent = "engine.aux.attribute.AttributeRate"
        min_change_location = "data/effect/discrete/flat_attribute_change/"

        change_ref_in_modpack = "effect.discrete.flat_attribute_change.min_heal.AoE2MinChangeAmount"
        change_raw_api_object = RawAPIObject(change_ref_in_modpack,
                                             "AoE2MinChangeAmount",
                                             api_objects,
                                             min_change_location)
        change_raw_api_object.set_filename("min_heal")
        change_raw_api_object.add_raw_parent(min_change_parent)

        attribute = ExpectedPointer(pregen_converter_group, "aux.attribute.types.Health")
        change_raw_api_object.add_raw_member("type",
                                             attribute,
                                             min_change_parent)
        change_raw_api_object.add_raw_member("rate",
                                             0,
                                             min_change_parent)

        pregen_converter_group.add_raw_api_object(change_raw_api_object)
        pregen_nyan_objects.update({change_ref_in_modpack: change_raw_api_object})

        # =======================================================================
        # Fallback effect for attacking (= minimum damage)
        # =======================================================================
        effect_parent = "engine.effect.discrete.flat_attribute_change.FlatAttributeChange"
        fallback_parent = "engine.effect.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"
        fallback_location = "data/effect/discrete/flat_attribute_change/"

        fallback_ref_in_modpack = "effect.discrete.flat_attribute_change.fallback.AoE2AttackFallback"
        fallback_raw_api_object = RawAPIObject(fallback_ref_in_modpack,
                                               "AoE2AttackFallback",
                                               api_objects,
                                               fallback_location)
        fallback_raw_api_object.set_filename("fallback")
        fallback_raw_api_object.add_raw_parent(fallback_parent)

        # Type
        type_ref = "engine.aux.attribute_change_type.type.Fallback"
        change_type = api_objects[type_ref]
        fallback_raw_api_object.add_raw_member("type",
                                               change_type,
                                               effect_parent)

        # Min value (optional)
        # =================================================================================
        amount_name = "%s.LowerCealing" % (fallback_ref_in_modpack)
        amount_raw_api_object = RawAPIObject(amount_name, "LowerCealing", api_objects)
        amount_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeAmount")
        amount_location = ExpectedPointer(pregen_converter_group, fallback_ref_in_modpack)
        amount_raw_api_object.set_location(amount_location)

        attribute = ExpectedPointer(pregen_converter_group, "aux.attribute.types.Health")
        amount_raw_api_object.add_raw_member("type",
                                             attribute,
                                             "engine.aux.attribute.AttributeAmount")
        amount_raw_api_object.add_raw_member("amount",
                                             1,
                                             "engine.aux.attribute.AttributeAmount")

        pregen_converter_group.add_raw_api_object(amount_raw_api_object)
        pregen_nyan_objects.update({amount_name: amount_raw_api_object})
        # =================================================================================
        amount_expected_pointer = ExpectedPointer(pregen_converter_group, amount_name)
        fallback_raw_api_object.add_raw_member("min_change_value",
                                               amount_expected_pointer,
                                               effect_parent)

        # Max value (optional; not needed

        # Change value
        # =================================================================================
        amount_name = "%s.ChangeAmount" % (fallback_ref_in_modpack)
        amount_raw_api_object = RawAPIObject(amount_name, "ChangeAmount", api_objects)
        amount_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeAmount")
        amount_location = ExpectedPointer(pregen_converter_group, fallback_ref_in_modpack)
        amount_raw_api_object.set_location(amount_location)

        attribute = ExpectedPointer(pregen_converter_group, "aux.attribute.types.Health")
        amount_raw_api_object.add_raw_member("type",
                                             attribute,
                                             "engine.aux.attribute.AttributeAmount")
        amount_raw_api_object.add_raw_member("amount",
                                             1,
                                             "engine.aux.attribute.AttributeAmount")

        pregen_converter_group.add_raw_api_object(amount_raw_api_object)
        pregen_nyan_objects.update({amount_name: amount_raw_api_object})

        # =================================================================================
        amount_expected_pointer = ExpectedPointer(pregen_converter_group, amount_name)
        fallback_raw_api_object.add_raw_member("change_value",
                                               amount_expected_pointer,
                                               effect_parent)

        # Ignore protection
        fallback_raw_api_object.add_raw_member("ignore_protection",
                                               [],
                                               effect_parent)

        pregen_converter_group.add_raw_api_object(fallback_raw_api_object)
        pregen_nyan_objects.update({fallback_ref_in_modpack: fallback_raw_api_object})

        # =======================================================================
        # Fallback resistance
        # =======================================================================
        effect_parent = "engine.resistance.discrete.flat_attribute_change.FlatAttributeChange"
        fallback_parent = "engine.resistance.discrete.flat_attribute_change.type.FlatAttributeChangeDecrease"
        fallback_location = "data/resistance/discrete/flat_attribute_change/"

        fallback_ref_in_modpack = "resistance.discrete.flat_attribute_change.fallback.AoE2AttackFallback"
        fallback_raw_api_object = RawAPIObject(fallback_ref_in_modpack,
                                               "AoE2AttackFallback",
                                               api_objects,
                                               fallback_location)
        fallback_raw_api_object.set_filename("fallback")
        fallback_raw_api_object.add_raw_parent(fallback_parent)

        # Type
        type_ref = "engine.aux.attribute_change_type.type.Fallback"
        change_type = api_objects[type_ref]
        fallback_raw_api_object.add_raw_member("type",
                                               change_type,
                                               effect_parent)

        # Block value
        # =================================================================================
        amount_name = "%s.BlockAmount" % (fallback_ref_in_modpack)
        amount_raw_api_object = RawAPIObject(amount_name, "BlockAmount", api_objects)
        amount_raw_api_object.add_raw_parent("engine.aux.attribute.AttributeAmount")
        amount_location = ExpectedPointer(pregen_converter_group, fallback_ref_in_modpack)
        amount_raw_api_object.set_location(amount_location)

        attribute = ExpectedPointer(pregen_converter_group, "aux.attribute.types.Health")
        amount_raw_api_object.add_raw_member("type",
                                             attribute,
                                             "engine.aux.attribute.AttributeAmount")
        amount_raw_api_object.add_raw_member("amount",
                                             0,
                                             "engine.aux.attribute.AttributeAmount")

        pregen_converter_group.add_raw_api_object(amount_raw_api_object)
        pregen_nyan_objects.update({amount_name: amount_raw_api_object})

        # =================================================================================
        amount_expected_pointer = ExpectedPointer(pregen_converter_group, amount_name)
        fallback_raw_api_object.add_raw_member("block_value",
                                               amount_expected_pointer,
                                               effect_parent)

        pregen_converter_group.add_raw_api_object(fallback_raw_api_object)
        pregen_nyan_objects.update({fallback_ref_in_modpack: fallback_raw_api_object})

    @staticmethod
    def _generate_terrain_types(full_data_set, pregen_converter_group):
        """
        Generate TerrainType objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        type_parent = "engine.aux.terrain_type.TerrainType"
        types_location = "data/aux/terrain_type/"

        terrain_type_lookups = TERRAIN_TYPE_LOOKUPS.values()

        for terrain_type in terrain_type_lookups:
            type_name = terrain_type[2]
            type_ref_in_modpack = "aux.terrain_type.types.%s" % (type_name)
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               type_name, api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    @staticmethod
    def _generate_resources(full_data_set, pregen_converter_group):
        """
        Generate Attribute objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        resource_parent = "engine.aux.resource.Resource"
        resources_location = "data/aux/resource/"

        # =======================================================================
        # Food
        # =======================================================================
        food_ref_in_modpack = "aux.resource.types.Food"
        food_raw_api_object = RawAPIObject(food_ref_in_modpack,
                                           "Food", api_objects,
                                           resources_location)
        food_raw_api_object.set_filename("types")
        food_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(food_raw_api_object)
        pregen_nyan_objects.update({food_ref_in_modpack: food_raw_api_object})

        food_raw_api_object.add_raw_member("max_storage",
                                           MemberSpecialValue.NYAN_INF,
                                           resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        food_name_ref_in_modpack = "aux.attribute.types.Food.FoodName"
        food_name_value = RawAPIObject(food_name_ref_in_modpack, "FoodName",
                                       api_objects, resources_location)
        food_name_value.set_filename("types")
        food_name_value.add_raw_parent(name_value_parent)
        food_name_value.add_raw_member("translations", [], name_value_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                food_name_ref_in_modpack)
        food_raw_api_object.add_raw_member("name",
                                           name_expected_pointer,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(food_name_value)
        pregen_nyan_objects.update({food_name_ref_in_modpack: food_name_value})

        # =======================================================================
        # Wood
        # =======================================================================
        wood_ref_in_modpack = "aux.resource.types.Wood"
        wood_raw_api_object = RawAPIObject(wood_ref_in_modpack,
                                           "Wood", api_objects,
                                           resources_location)
        wood_raw_api_object.set_filename("types")
        wood_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(wood_raw_api_object)
        pregen_nyan_objects.update({wood_ref_in_modpack: wood_raw_api_object})

        wood_raw_api_object.add_raw_member("max_storage",
                                           MemberSpecialValue.NYAN_INF,
                                           resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        wood_name_ref_in_modpack = "aux.attribute.types.Wood.WoodName"
        wood_name_value = RawAPIObject(wood_name_ref_in_modpack, "WoodName",
                                       api_objects, resources_location)
        wood_name_value.set_filename("types")
        wood_name_value.add_raw_parent(name_value_parent)
        wood_name_value.add_raw_member("translations", [], name_value_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                wood_name_ref_in_modpack)
        wood_raw_api_object.add_raw_member("name",
                                           name_expected_pointer,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(wood_name_value)
        pregen_nyan_objects.update({wood_name_ref_in_modpack: wood_name_value})

        # =======================================================================
        # Stone
        # =======================================================================
        stone_ref_in_modpack = "aux.resource.types.Stone"
        stone_raw_api_object = RawAPIObject(stone_ref_in_modpack,
                                            "Stone", api_objects,
                                            resources_location)
        stone_raw_api_object.set_filename("types")
        stone_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(stone_raw_api_object)
        pregen_nyan_objects.update({stone_ref_in_modpack: stone_raw_api_object})

        stone_raw_api_object.add_raw_member("max_storage",
                                            MemberSpecialValue.NYAN_INF,
                                            resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        stone_name_ref_in_modpack = "aux.attribute.types.Stone.StoneName"
        stone_name_value = RawAPIObject(stone_name_ref_in_modpack, "StoneName",
                                        api_objects, resources_location)
        stone_name_value.set_filename("types")
        stone_name_value.add_raw_parent(name_value_parent)
        stone_name_value.add_raw_member("translations", [], name_value_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                stone_name_ref_in_modpack)
        stone_raw_api_object.add_raw_member("name",
                                            name_expected_pointer,
                                            resource_parent)

        pregen_converter_group.add_raw_api_object(stone_name_value)
        pregen_nyan_objects.update({stone_name_ref_in_modpack: stone_name_value})

        # =======================================================================
        # Gold
        # =======================================================================
        gold_ref_in_modpack = "aux.resource.types.Gold"
        gold_raw_api_object = RawAPIObject(gold_ref_in_modpack,
                                           "Gold", api_objects,
                                           resources_location)
        gold_raw_api_object.set_filename("types")
        gold_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(gold_raw_api_object)
        pregen_nyan_objects.update({gold_ref_in_modpack: gold_raw_api_object})

        gold_raw_api_object.add_raw_member("max_storage",
                                           MemberSpecialValue.NYAN_INF,
                                           resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        gold_name_ref_in_modpack = "aux.attribute.types.Gold.GoldName"
        gold_name_value = RawAPIObject(gold_name_ref_in_modpack, "GoldName",
                                       api_objects, resources_location)
        gold_name_value.set_filename("types")
        gold_name_value.add_raw_parent(name_value_parent)
        gold_name_value.add_raw_member("translations", [], name_value_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                gold_name_ref_in_modpack)
        gold_raw_api_object.add_raw_member("name",
                                           name_expected_pointer,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(gold_name_value)
        pregen_nyan_objects.update({gold_name_ref_in_modpack: gold_name_value})

        # =======================================================================
        # Population Space
        # =======================================================================
        resource_contingent_parent = "engine.aux.resource.ResourceContingent"

        pop_ref_in_modpack = "aux.resource.types.PopulationSpace"
        pop_raw_api_object = RawAPIObject(pop_ref_in_modpack,
                                          "PopulationSpace", api_objects,
                                          resources_location)
        pop_raw_api_object.set_filename("types")
        pop_raw_api_object.add_raw_parent(resource_contingent_parent)

        pregen_converter_group.add_raw_api_object(pop_raw_api_object)
        pregen_nyan_objects.update({pop_ref_in_modpack: pop_raw_api_object})

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        pop_name_ref_in_modpack = "aux.attribute.types.PopulationSpace.PopulationSpaceName"
        pop_name_value = RawAPIObject(pop_name_ref_in_modpack, "PopulationSpaceName",
                                      api_objects, resources_location)
        pop_name_value.set_filename("types")
        pop_name_value.add_raw_parent(name_value_parent)
        pop_name_value.add_raw_member("translations", [], name_value_parent)

        name_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                pop_name_ref_in_modpack)
        pop_raw_api_object.add_raw_member("name",
                                          name_expected_pointer,
                                          resource_parent)
        pop_raw_api_object.add_raw_member("max_storage",
                                          MemberSpecialValue.NYAN_INF,
                                          resource_parent)
        pop_raw_api_object.add_raw_member("min_amount",
                                          0,
                                          resource_contingent_parent)
        pop_raw_api_object.add_raw_member("max_amount",
                                          200,
                                          resource_contingent_parent)

        pregen_converter_group.add_raw_api_object(pop_name_value)
        pregen_nyan_objects.update({pop_name_ref_in_modpack: pop_name_value})

    @staticmethod
    def _generate_death_condition(full_data_set, pregen_converter_group):
        """
        Generate DeathCondition objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ExpectedPointer
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # Death condition
        # =======================================================================
        clause_parent = "engine.aux.boolean.Clause"
        clause_location = "data/aux/boolean/clause/death/"

        death_ref_in_modpack = "aux.boolean.clause.death.StandardHealthDeath"
        clause_raw_api_object = RawAPIObject(death_ref_in_modpack,
                                             "StandardHealthDeath",
                                             api_objects,
                                             clause_location)
        clause_raw_api_object.set_filename("death")
        clause_raw_api_object.add_raw_parent(clause_parent)

        # Literals (see below)
        literals_expected_pointer = [ExpectedPointer(pregen_converter_group,
                                                     "aux.boolean.clause.death.StandardHealthDeathLiteral")]
        clause_raw_api_object.add_raw_member("literals",
                                             literals_expected_pointer,
                                             clause_parent)

        # Requirement mode does not matter, so we use ANY
        requirement_mode = api_objects["engine.aux.boolean.requirement_mode.type.Any"]
        clause_raw_api_object.add_raw_member("clause_requirement",
                                             requirement_mode,
                                             clause_parent)

        # Clause will not default to 'True' when it was fulfilled once
        clause_raw_api_object.add_raw_member("only_once", False, clause_parent)

        pregen_converter_group.add_raw_api_object(clause_raw_api_object)
        pregen_nyan_objects.update({death_ref_in_modpack: clause_raw_api_object})

        # Literal
        literal_parent = "engine.aux.boolean.Literal"
        interval_parent = "engine.aux.boolean.literal.type.AttributeBelowValue"

        death_literal_ref_in_modpack = "aux.boolean.clause.death.StandardHealthDeathLiteral"
        literal_raw_api_object = RawAPIObject(death_literal_ref_in_modpack,
                                              "StandardHealthDeathLiteral",
                                              api_objects,
                                              clause_location)
        literal_location = ExpectedPointer(pregen_converter_group, death_ref_in_modpack)
        literal_raw_api_object.set_location(literal_location)
        literal_raw_api_object.add_raw_parent(interval_parent)

        health_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                  "aux.attribute.types.Health")
        literal_raw_api_object.add_raw_member("mode",
                                              True,
                                              literal_parent)
        scope_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                 "aux.boolean.clause.death.StandardHealthDeathScope")
        literal_raw_api_object.add_raw_member("scope",
                                              scope_expected_pointer,
                                              literal_parent)
        literal_raw_api_object.add_raw_member("attribute",
                                              health_expected_pointer,
                                              interval_parent)
        # sidenote: Apparently this is actually HP<1 in Genie
        # (https://youtu.be/FdBk8zGbE7U?t=7m16s)
        literal_raw_api_object.add_raw_member("threshold",
                                              1,
                                              interval_parent)

        pregen_converter_group.add_raw_api_object(literal_raw_api_object)
        pregen_nyan_objects.update({death_literal_ref_in_modpack: literal_raw_api_object})

        # LiteralScope
        scope_parent = "engine.aux.boolean.literal_scope.LiteralScope"
        self_scope_parent = "engine.aux.boolean.literal_scope.type.Self"

        death_scope_ref_in_modpack = "aux.boolean.clause.death.StandardHealthDeathScope"
        scope_raw_api_object = RawAPIObject(death_scope_ref_in_modpack,
                                            "StandardHealthDeathScope",
                                            api_objects,
                                            clause_location)
        scope_location = ExpectedPointer(pregen_converter_group, death_ref_in_modpack)
        scope_raw_api_object.set_location(scope_location)
        scope_raw_api_object.add_raw_parent(self_scope_parent)

        scope_diplomatic_stances = [api_objects["engine.aux.diplomatic_stance.type.Self"]]
        scope_raw_api_object.add_raw_member("diplomatic_stances",
                                            scope_diplomatic_stances,
                                            scope_parent)

        pregen_converter_group.add_raw_api_object(scope_raw_api_object)
        pregen_nyan_objects.update({death_scope_ref_in_modpack: scope_raw_api_object})

        # =======================================================================
        # Garrison empty condition
        # =======================================================================
        clause_parent = "engine.aux.boolean.Clause"
        clause_location = "data/aux/boolean/clause/garrison_empty/"

        death_ref_in_modpack = "aux.boolean.clause.death.BuildingDamageEmpty"
        clause_raw_api_object = RawAPIObject(death_ref_in_modpack,
                                             "BuildingDamageEmpty",
                                             api_objects,
                                             clause_location)
        clause_raw_api_object.set_filename("building_damage_empty")
        clause_raw_api_object.add_raw_parent(clause_parent)

        # Literals (see below)
        literals_expected_pointer = [ExpectedPointer(pregen_converter_group,
                                                     "aux.boolean.clause.death.BuildingDamageEmptyLiteral")]
        clause_raw_api_object.add_raw_member("literals",
                                             literals_expected_pointer,
                                             clause_parent)

        # Requirement mode does not matter, so we use ANY
        requirement_mode = api_objects["engine.aux.boolean.requirement_mode.type.Any"]
        clause_raw_api_object.add_raw_member("clause_requirement",
                                             requirement_mode,
                                             clause_parent)

        # Clause will not default to 'True' when it was fulfilled once
        clause_raw_api_object.add_raw_member("only_once", False, clause_parent)

        pregen_converter_group.add_raw_api_object(clause_raw_api_object)
        pregen_nyan_objects.update({death_ref_in_modpack: clause_raw_api_object})

        # Literal
        literal_parent = "engine.aux.boolean.Literal"
        interval_parent = "engine.aux.boolean.literal.type.AttributeBelowValue"

        death_literal_ref_in_modpack = "aux.boolean.clause.death.BuildingDamageEmptyLiteral"
        literal_raw_api_object = RawAPIObject(death_literal_ref_in_modpack,
                                              "BuildingDamageEmptyLiteral",
                                              api_objects,
                                              clause_location)
        literal_location = ExpectedPointer(pregen_converter_group, death_ref_in_modpack)
        literal_raw_api_object.set_location(literal_location)
        literal_raw_api_object.add_raw_parent(interval_parent)

        health_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                  "aux.attribute.types.Health")
        literal_raw_api_object.add_raw_member("mode",
                                              True,
                                              literal_parent)
        scope_expected_pointer = ExpectedPointer(pregen_converter_group,
                                                 "aux.boolean.clause.death.BuildingDamageEmptyScope")
        literal_raw_api_object.add_raw_member("scope",
                                              scope_expected_pointer,
                                              literal_parent)
        literal_raw_api_object.add_raw_member("attribute",
                                              health_expected_pointer,
                                              interval_parent)
        literal_raw_api_object.add_raw_member("threshold",
                                              0.2,
                                              interval_parent)

        pregen_converter_group.add_raw_api_object(literal_raw_api_object)
        pregen_nyan_objects.update({death_literal_ref_in_modpack: literal_raw_api_object})

        # LiteralScope
        scope_parent = "engine.aux.boolean.literal_scope.LiteralScope"
        self_scope_parent = "engine.aux.boolean.literal_scope.type.Self"

        death_scope_ref_in_modpack = "aux.boolean.clause.death.BuildingDamageEmptyScope"
        scope_raw_api_object = RawAPIObject(death_scope_ref_in_modpack,
                                            "BuildingDamageEmptyScope",
                                            api_objects,
                                            clause_location)
        scope_location = ExpectedPointer(pregen_converter_group, death_ref_in_modpack)
        scope_raw_api_object.set_location(scope_location)
        scope_raw_api_object.add_raw_parent(self_scope_parent)

        scope_diplomatic_stances = [api_objects["engine.aux.diplomatic_stance.type.Self"]]
        scope_raw_api_object.add_raw_member("diplomatic_stances",
                                            scope_diplomatic_stances,
                                            scope_parent)

        pregen_converter_group.add_raw_api_object(scope_raw_api_object)
        pregen_nyan_objects.update({death_scope_ref_in_modpack: scope_raw_api_object})

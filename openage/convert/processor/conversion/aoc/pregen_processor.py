# Copyright 2020-2021 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-lines,too-many-locals,too-many-statements
#
# TODO:
# pylint: disable=line-too-long

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.
"""
from .....nyan.nyan_structs import MemberSpecialValue
from ....entity_object.conversion.converter_object import RawAPIObject,\
    ConverterObjectGroup
from ....service.conversion import internal_name_lookups
from ....value_object.conversion.forward_ref import ForwardRef


class AoCPregenSubprocessor:
    """
    Creates raw API objects for hardcoded settings in AoC.
    """

    @classmethod
    def generate(cls, gamedata):
        """
        Create nyan objects for hardcoded properties.
        """
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        cls.generate_attributes(gamedata, pregen_converter_group)
        cls.generate_diplomatic_stances(gamedata, pregen_converter_group)
        cls.generate_team_property(gamedata, pregen_converter_group)
        cls.generate_entity_types(gamedata, pregen_converter_group)
        cls.generate_effect_types(gamedata, pregen_converter_group)
        cls.generate_exchange_objects(gamedata, pregen_converter_group)
        cls.generate_formation_types(gamedata, pregen_converter_group)
        cls.generate_language_objects(gamedata, pregen_converter_group)
        cls.generate_misc_effect_objects(gamedata, pregen_converter_group)
        cls.generate_modifiers(gamedata, pregen_converter_group)
        cls.generate_terrain_types(gamedata, pregen_converter_group)
        cls.generate_resources(gamedata, pregen_converter_group)
        cls.generate_death_condition(gamedata, pregen_converter_group)

        pregen_nyan_objects = gamedata.pregen_nyan_objects
        # Create nyan objects from the raw API objects
        for pregen_object in pregen_nyan_objects.values():
            pregen_object.create_nyan_object()

        # This has to be a separate for-loop because of possible object interdependencies
        for pregen_object in pregen_nyan_objects.values():
            pregen_object.create_nyan_members()

            if not pregen_object.is_ready():
                raise Exception("%s: Pregenerated object is not ready for export. "
                                "Member or object not initialized." % (pregen_object))

    @staticmethod
    def generate_attributes(full_data_set, pregen_converter_group):
        """
        Generate Attribute objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # TODO: Fill translations
        # =======================================================================
        attribute_parent = "engine.util.attribute.Attribute"
        attributes_location = "data/util/attribute/"

        # =======================================================================
        # HP
        # =======================================================================
        health_ref_in_modpack = "util.attribute.types.Health"
        health_raw_api_object = RawAPIObject(health_ref_in_modpack,
                                             "Health", api_objects,
                                             attributes_location)
        health_raw_api_object.set_filename("types")
        health_raw_api_object.add_raw_parent(attribute_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      "util.attribute.types.Health.HealthName")
        health_raw_api_object.add_raw_member("name", name_forward_ref,
                                             attribute_parent)
        abbrv_forward_ref = ForwardRef(pregen_converter_group,
                                       "util.attribute.types.Health.HealthAbbreviation")
        health_raw_api_object.add_raw_member("abbreviation", abbrv_forward_ref,
                                             attribute_parent)

        pregen_converter_group.add_raw_api_object(health_raw_api_object)
        pregen_nyan_objects.update({health_ref_in_modpack: health_raw_api_object})

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        health_name_ref_in_modpack = "util.attribute.types.Health.HealthName"
        health_name_value = RawAPIObject(health_name_ref_in_modpack, "HealthName",
                                         api_objects, attributes_location)
        health_name_value.set_filename("types")
        health_name_value.add_raw_parent(name_value_parent)
        health_name_value.add_raw_member("translations", [], name_value_parent)

        pregen_converter_group.add_raw_api_object(health_name_value)
        pregen_nyan_objects.update({health_name_ref_in_modpack: health_name_value})

        abbrv_value_parent = "engine.util.language.translated.type.TranslatedString"
        health_abbrv_ref_in_modpack = "util.attribute.types.Health.HealthAbbreviation"
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
        faith_ref_in_modpack = "util.attribute.types.Faith"
        faith_raw_api_object = RawAPIObject(faith_ref_in_modpack,
                                            "Faith", api_objects,
                                            attributes_location)
        faith_raw_api_object.set_filename("types")
        faith_raw_api_object.add_raw_parent(attribute_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      "util.attribute.types.Faith.FaithName")
        faith_raw_api_object.add_raw_member("name", name_forward_ref,
                                            attribute_parent)
        abbrv_forward_ref = ForwardRef(pregen_converter_group,
                                       "util.attribute.types.Faith.FaithAbbreviation")
        faith_raw_api_object.add_raw_member("abbreviation", abbrv_forward_ref,
                                            attribute_parent)

        pregen_converter_group.add_raw_api_object(faith_raw_api_object)
        pregen_nyan_objects.update({faith_ref_in_modpack: faith_raw_api_object})

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        faith_name_ref_in_modpack = "util.attribute.types.Faith.FaithName"
        faith_name_value = RawAPIObject(faith_name_ref_in_modpack, "FaithName",
                                        api_objects, attributes_location)
        faith_name_value.set_filename("types")
        faith_name_value.add_raw_parent(name_value_parent)
        faith_name_value.add_raw_member("translations", [], name_value_parent)

        pregen_converter_group.add_raw_api_object(faith_name_value)
        pregen_nyan_objects.update({faith_name_ref_in_modpack: faith_name_value})

        abbrv_value_parent = "engine.util.language.translated.type.TranslatedString"
        faith_abbrv_ref_in_modpack = "util.attribute.types.Faith.FaithAbbreviation"
        faith_abbrv_value = RawAPIObject(faith_abbrv_ref_in_modpack, "FaithAbbreviation",
                                         api_objects, attributes_location)
        faith_abbrv_value.set_filename("types")
        faith_abbrv_value.add_raw_parent(abbrv_value_parent)
        faith_abbrv_value.add_raw_member("translations", [], abbrv_value_parent)

        pregen_converter_group.add_raw_api_object(faith_abbrv_value)
        pregen_nyan_objects.update({faith_abbrv_ref_in_modpack: faith_abbrv_value})

    @staticmethod
    def generate_diplomatic_stances(full_data_set, pregen_converter_group):
        """
        Generate DiplomaticStance objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        stance_parent = "engine.util.diplomatic_stance.DiplomaticStance"
        stance_location = "data/util/diplomatic_stance/"

        # =======================================================================
        # Enemy
        # =======================================================================
        enemy_ref_in_modpack = "util.diplomatic_stance.types.Enemy"
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
        neutral_ref_in_modpack = "util.diplomatic_stance.types.Neutral"
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
        friendly_ref_in_modpack = "util.diplomatic_stance.types.Friendly"
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
        gaia_ref_in_modpack = "util.diplomatic_stance.types.Gaia"
        gaia_raw_api_object = RawAPIObject(gaia_ref_in_modpack,
                                           "Gaia", api_objects,
                                           stance_location)
        gaia_raw_api_object.set_filename("types")
        gaia_raw_api_object.add_raw_parent(stance_parent)

        pregen_converter_group.add_raw_api_object(gaia_raw_api_object)
        pregen_nyan_objects.update({gaia_ref_in_modpack: gaia_raw_api_object})

    @staticmethod
    def generate_team_property(full_data_set, pregen_converter_group):
        """
        Generate the property used in team patches objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        prop_ref_in_modpack = "util.patch.property.types.Team"
        prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                           "Team",
                                           api_objects,
                                           "data/util/patch/property/")
        prop_raw_api_object.set_filename("types")
        prop_raw_api_object.add_raw_parent("engine.util.patch.property.type.Diplomatic")

        pregen_converter_group.add_raw_api_object(prop_raw_api_object)
        pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

        stances = [
            full_data_set.nyan_api_objects["engine.util.diplomatic_stance.type.Self"],
            ForwardRef(pregen_converter_group, "util.diplomatic_stance.types.Friendly")
        ]
        prop_raw_api_object.add_raw_member("stances",
                                           stances,
                                           "engine.util.patch.property.type.Diplomatic")

    @staticmethod
    def generate_entity_types(full_data_set, pregen_converter_group):
        """
        Generate GameEntityType objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        class_lookup_dict = internal_name_lookups.get_class_lookups(full_data_set.game_version)

        type_parent = "engine.util.game_entity_type.GameEntityType"
        types_location = "data/util/game_entity_type/"

        # =======================================================================
        # Ambient
        # =======================================================================
        ambient_ref_in_modpack = "util.game_entity_type.types.Ambient"
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
        building_ref_in_modpack = "util.game_entity_type.types.Building"
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
        item_ref_in_modpack = "util.game_entity_type.types.Item"
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
        projectile_ref_in_modpack = "util.game_entity_type.types.Projectile"
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
        unit_ref_in_modpack = "util.game_entity_type.types.Unit"
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
        drop_site_ref_in_modpack = "util.game_entity_type.types.DropSite"
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
            class_name = class_lookup_dict[unit_class]
            class_obj_name = f"util.game_entity_type.types.{class_name}"

            new_game_entity_type = RawAPIObject(class_obj_name, class_name,
                                                full_data_set.nyan_api_objects,
                                                types_location)
            new_game_entity_type.set_filename("types")
            new_game_entity_type.add_raw_parent("engine.util.game_entity_type.GameEntityType")
            new_game_entity_type.create_nyan_object()

            pregen_converter_group.add_raw_api_object(new_game_entity_type)
            pregen_nyan_objects.update({class_obj_name: new_game_entity_type})

    @staticmethod
    def generate_effect_types(full_data_set, pregen_converter_group):
        """
        Generate types for effects and resistances.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        name_lookup_dict = internal_name_lookups.get_entity_lookups(full_data_set.game_version)
        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(full_data_set.game_version)

        # =======================================================================
        # Armor types
        # =======================================================================
        type_parent = "engine.util.attribute_change_type.AttributeChangeType"
        types_location = "data/util/attribute_change_type/"

        for type_name in armor_lookup_dict.values():
            type_ref_in_modpack = f"util.attribute_change_type.types.{type_name}"
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
        type_ref_in_modpack = "util.attribute_change_type.types.Heal"
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
            if unit_line.is_repairable():
                repairable_lines.append(unit_line)

        for repairable_line in repairable_lines:
            game_entity_name = name_lookup_dict[repairable_line.get_head_unit_id()][0]

            type_ref_in_modpack = f"util.attribute_change_type.types.{game_entity_name}Repair"
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               f"{game_entity_name}Repair",
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
            game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

            type_ref_in_modpack = f"util.attribute_change_type.types.{game_entity_name}Construct"
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               f"{game_entity_name}Construct",
                                               api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        type_parent = "engine.util.progress_type.type.Construct"
        types_location = "data/util/construct_type/"

        for constructable_line in constructable_lines:
            game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

            type_ref_in_modpack = f"util.construct_type.types.{game_entity_name}Construct"
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               f"{game_entity_name}Construct",
                                               api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

        # =======================================================================
        # ConvertType: UnitConvert
        # =======================================================================
        type_parent = "engine.util.convert_type.ConvertType"
        types_location = "data/util/convert_type/"

        type_ref_in_modpack = "util.convert_type.types.UnitConvert"
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
        type_parent = "engine.util.convert_type.ConvertType"
        types_location = "data/util/convert_type/"

        type_ref_in_modpack = "util.convert_type.types.BuildingConvert"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           "BuildingConvert", api_objects,
                                           types_location)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    @staticmethod
    def generate_exchange_objects(full_data_set, pregen_converter_group):
        """
        Generate objects for market trading (ExchangeResources).

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # Exchange mode Buy
        # =======================================================================
        exchange_mode_parent = "engine.util.exchange_mode.type.Buy"
        exchange_mode_location = "data/util/resource/"

        exchange_mode_ref_in_modpack = "util.resource.market_trading.MarketBuyExchangeMode"
        exchange_mode_raw_api_object = RawAPIObject(exchange_mode_ref_in_modpack,
                                                    "MarketBuyExchangePool",
                                                    api_objects,
                                                    exchange_mode_location)
        exchange_mode_raw_api_object.set_filename("market_trading")
        exchange_mode_raw_api_object.add_raw_parent(exchange_mode_parent)

        # Fee (30% on top)
        exchange_mode_raw_api_object.add_raw_member("fee_multiplier",
                                                    1.3,
                                                    "engine.util.exchange_mode.ExchangeMode")

        pregen_converter_group.add_raw_api_object(exchange_mode_raw_api_object)
        pregen_nyan_objects.update({exchange_mode_ref_in_modpack: exchange_mode_raw_api_object})

        # =======================================================================
        # Exchange mode Sell
        # =======================================================================
        exchange_mode_parent = "engine.util.exchange_mode.type.Sell"
        exchange_mode_location = "data/util/resource/"

        exchange_mode_ref_in_modpack = "util.resource.market_trading.MarketSellExchangeMode"
        exchange_mode_raw_api_object = RawAPIObject(exchange_mode_ref_in_modpack,
                                                    "MarketSellExchangeMode",
                                                    api_objects,
                                                    exchange_mode_location)
        exchange_mode_raw_api_object.set_filename("market_trading")
        exchange_mode_raw_api_object.add_raw_parent(exchange_mode_parent)

        # Fee (30% reduced)
        exchange_mode_raw_api_object.add_raw_member("fee_multiplier",
                                                    0.7,
                                                    "engine.util.exchange_mode.ExchangeMode")

        pregen_converter_group.add_raw_api_object(exchange_mode_raw_api_object)
        pregen_nyan_objects.update({exchange_mode_ref_in_modpack: exchange_mode_raw_api_object})

        # =======================================================================
        # Market Food price pool
        # =======================================================================
        exchange_pool_parent = "engine.util.price_pool.PricePool"
        exchange_pool_location = "data/util/resource/"

        exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketFoodPricePool"
        exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                    "MarketFoodPricePool",
                                                    api_objects,
                                                    exchange_pool_location)
        exchange_pool_raw_api_object.set_filename("market_trading")
        exchange_pool_raw_api_object.add_raw_parent(exchange_pool_parent)

        pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
        pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})

        # =======================================================================
        # Market Wood price pool
        # =======================================================================
        exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketWoodPricePool"
        exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                    "MarketWoodPricePool",
                                                    api_objects,
                                                    exchange_pool_location)
        exchange_pool_raw_api_object.set_filename("market_trading")
        exchange_pool_raw_api_object.add_raw_parent(exchange_pool_parent)

        pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
        pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})

        # =======================================================================
        # Market Stone price pool
        # =======================================================================
        exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketStonePricePool"
        exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                    "MarketStonePricePool",
                                                    api_objects,
                                                    exchange_pool_location)
        exchange_pool_raw_api_object.set_filename("market_trading")
        exchange_pool_raw_api_object.add_raw_parent(exchange_pool_parent)

        pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
        pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})

        # =======================================================================
        # Exchange rate Food
        # =======================================================================
        exchange_rate_parent = "engine.util.exchange_rate.ExchangeRate"
        exchange_rate_location = "data/util/resource/"

        exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketFoodExchangeRate"
        exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                    "MarketFoodExchangeRate",
                                                    api_objects,
                                                    exchange_rate_location)
        exchange_rate_raw_api_object.set_filename("market_trading")
        exchange_rate_raw_api_object.add_raw_parent(exchange_rate_parent)

        # Base price
        exchange_rate_raw_api_object.add_raw_member("base_price",
                                                    1.0,
                                                    exchange_rate_parent)

        # Price adjust methods
        pa_buy_forward_ref = ForwardRef(pregen_converter_group,
                                        "util.resource.market_trading.MarketBuyPriceMode")
        pa_sell_forward_ref = ForwardRef(pregen_converter_group,
                                         "util.resource.market_trading.MarketSellPriceMode")
        price_adjust = {
            api_objects["engine.util.exchange_mode.type.Buy"]: pa_buy_forward_ref,
            api_objects["engine.util.exchange_mode.type.Sell"]: pa_sell_forward_ref
        }
        exchange_rate_raw_api_object.add_raw_member("price_adjust",
                                                    price_adjust,
                                                    exchange_rate_parent)

        # Price pool
        pool_forward_ref = ForwardRef(pregen_converter_group,
                                      "util.resource.market_trading.MarketFoodPricePool")
        exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                    pool_forward_ref,
                                                    exchange_rate_parent)

        pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
        pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})

        # =======================================================================
        # Exchange rate Wood
        # =======================================================================
        exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketWoodExchangeRate"
        exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                    "MarketWoodExchangeRate",
                                                    api_objects,
                                                    exchange_rate_location)
        exchange_rate_raw_api_object.set_filename("market_trading")
        exchange_rate_raw_api_object.add_raw_parent(exchange_rate_parent)

        # Base price
        exchange_rate_raw_api_object.add_raw_member("base_price",
                                                    1.0,
                                                    exchange_rate_parent)

        # Price adjust methods
        pa_buy_forward_ref = ForwardRef(pregen_converter_group,
                                        "util.resource.market_trading.MarketBuyPriceMode")
        pa_sell_forward_ref = ForwardRef(pregen_converter_group,
                                         "util.resource.market_trading.MarketSellPriceMode")
        price_adjust = {
            api_objects["engine.util.exchange_mode.type.Buy"]: pa_buy_forward_ref,
            api_objects["engine.util.exchange_mode.type.Sell"]: pa_sell_forward_ref
        }
        exchange_rate_raw_api_object.add_raw_member("price_adjust",
                                                    price_adjust,
                                                    exchange_rate_parent)

        # Price pool
        pool_forward_ref = ForwardRef(pregen_converter_group,
                                      "util.resource.market_trading.MarketWoodPricePool")
        exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                    pool_forward_ref,
                                                    exchange_rate_parent)

        pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
        pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})

        # =======================================================================
        # Exchange rate Stone
        # =======================================================================
        exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketStoneExchangeRate"
        exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                    "MarketStoneExchangeRate",
                                                    api_objects,
                                                    exchange_rate_location)
        exchange_rate_raw_api_object.set_filename("market_trading")
        exchange_rate_raw_api_object.add_raw_parent(exchange_rate_parent)

        # Base price
        exchange_rate_raw_api_object.add_raw_member("base_price",
                                                    1.3,
                                                    exchange_rate_parent)

        # Price adjust methods
        pa_buy_forward_ref = ForwardRef(pregen_converter_group,
                                        "util.resource.market_trading.MarketBuyPriceMode")
        pa_sell_forward_ref = ForwardRef(pregen_converter_group,
                                         "util.resource.market_trading.MarketSellPriceMode")
        price_adjust = {
            api_objects["engine.util.exchange_mode.type.Buy"]: pa_buy_forward_ref,
            api_objects["engine.util.exchange_mode.type.Sell"]: pa_sell_forward_ref
        }
        exchange_rate_raw_api_object.add_raw_member("price_adjust",
                                                    price_adjust,
                                                    exchange_rate_parent)

        # Price pool
        pool_forward_ref = ForwardRef(pregen_converter_group,
                                      "util.resource.market_trading.MarketStonePricePool")
        exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                    pool_forward_ref,
                                                    exchange_rate_parent)

        pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
        pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})

        # =======================================================================
        # Buy Price mode
        # =======================================================================
        price_mode_parent = "engine.util.price_mode.type.Dynamic"
        price_mode_location = "data/util/resource/"

        price_mode_ref_in_modpack = "util.resource.market_trading.MarketBuyPriceMode"
        price_mode_raw_api_object = RawAPIObject(price_mode_ref_in_modpack,
                                                 "MarketBuyPriceMode",
                                                 api_objects,
                                                 price_mode_location)
        price_mode_raw_api_object.set_filename("market_trading")
        price_mode_raw_api_object.add_raw_parent(price_mode_parent)

        # Min price
        price_mode_raw_api_object.add_raw_member("change_value",
                                                 0.03,
                                                 price_mode_parent)

        # Min price
        price_mode_raw_api_object.add_raw_member("min_price",
                                                 0.3,
                                                 price_mode_parent)

        # Max price
        price_mode_raw_api_object.add_raw_member("max_price",
                                                 99.9,
                                                 price_mode_parent)

        pregen_converter_group.add_raw_api_object(price_mode_raw_api_object)
        pregen_nyan_objects.update({price_mode_ref_in_modpack: price_mode_raw_api_object})

        # =======================================================================
        # Sell Price mode
        # =======================================================================
        price_mode_parent = "engine.util.price_mode.type.Dynamic"
        price_mode_location = "data/util/resource/"

        price_mode_ref_in_modpack = "util.resource.market_trading.MarketSellPriceMode"
        price_mode_raw_api_object = RawAPIObject(price_mode_ref_in_modpack,
                                                 "MarketSellPriceMode",
                                                 api_objects,
                                                 price_mode_location)
        price_mode_raw_api_object.set_filename("market_trading")
        price_mode_raw_api_object.add_raw_parent(price_mode_parent)

        # Min price
        price_mode_raw_api_object.add_raw_member("change_value",
                                                 -0.03,
                                                 price_mode_parent)

        # Min price
        price_mode_raw_api_object.add_raw_member("min_price",
                                                 0.3,
                                                 price_mode_parent)

        # Max price
        price_mode_raw_api_object.add_raw_member("max_price",
                                                 99.9,
                                                 price_mode_parent)

        pregen_converter_group.add_raw_api_object(price_mode_raw_api_object)
        pregen_nyan_objects.update({price_mode_ref_in_modpack: price_mode_raw_api_object})

    @staticmethod
    def generate_formation_types(full_data_set, pregen_converter_group):
        """
        Generate Formation and Subformation objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # Line formation
        # =======================================================================
        formation_parent = "engine.util.formation.Formation"
        formation_location = "data/util/formation/"

        formation_ref_in_modpack = "util.formation.types.Line"
        formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                                "Line",
                                                api_objects,
                                                formation_location)
        formation_raw_api_object.set_filename("types")
        formation_raw_api_object.add_raw_parent(formation_parent)

        subformations = [
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
        ]
        formation_raw_api_object.add_raw_member("subformations",
                                                subformations,
                                                formation_parent)

        pregen_converter_group.add_raw_api_object(formation_raw_api_object)
        pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})
        # =======================================================================
        # Staggered formation
        # =======================================================================
        formation_ref_in_modpack = "util.formation.types.Staggered"
        formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                                "Staggered",
                                                api_objects,
                                                formation_location)
        formation_raw_api_object.set_filename("types")
        formation_raw_api_object.add_raw_parent(formation_parent)

        subformations = [
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
        ]
        formation_raw_api_object.add_raw_member("subformations",
                                                subformations,
                                                formation_parent)

        pregen_converter_group.add_raw_api_object(formation_raw_api_object)
        pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})
        # =======================================================================
        # Box formation
        # =======================================================================
        formation_ref_in_modpack = "util.formation.types.Box"
        formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                                "Box",
                                                api_objects,
                                                formation_location)
        formation_raw_api_object.set_filename("types")
        formation_raw_api_object.add_raw_parent(formation_parent)

        subformations = [
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
        ]
        formation_raw_api_object.add_raw_member("subformations",
                                                subformations,
                                                formation_parent)

        pregen_converter_group.add_raw_api_object(formation_raw_api_object)
        pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})
        # =======================================================================
        # Flank formation
        # =======================================================================
        formation_ref_in_modpack = "util.formation.types.Flank"
        formation_raw_api_object = RawAPIObject(formation_ref_in_modpack,
                                                "Flank",
                                                api_objects,
                                                formation_location)
        formation_raw_api_object.set_filename("types")
        formation_raw_api_object.add_raw_parent(formation_parent)

        subformations = [
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Cavalry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Infantry"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Ranged"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Siege"),
            ForwardRef(pregen_converter_group, "util.formation.subformation.types.Support"),
        ]
        formation_raw_api_object.add_raw_member("subformations",
                                                subformations,
                                                formation_parent)

        pregen_converter_group.add_raw_api_object(formation_raw_api_object)
        pregen_nyan_objects.update({formation_ref_in_modpack: formation_raw_api_object})

        # =======================================================================
        # Cavalry subformation
        # =======================================================================
        subformation_parent = "engine.util.formation.Subformation"
        subformation_location = "data/util/formation/"

        subformation_ref_in_modpack = "util.formation.subformation.types.Cavalry"
        subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                                   "Cavalry",
                                                   api_objects,
                                                   subformation_location)
        subformation_raw_api_object.set_filename("subformations")
        subformation_raw_api_object.add_raw_parent(subformation_parent)

        subformation_raw_api_object.add_raw_member("ordering_priority",
                                                   5,
                                                   subformation_parent)

        pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
        pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

        # =======================================================================
        # Infantry subformation
        # =======================================================================
        subformation_ref_in_modpack = "util.formation.subformation.types.Infantry"
        subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                                   "Infantry",
                                                   api_objects,
                                                   subformation_location)
        subformation_raw_api_object.set_filename("subformations")
        subformation_raw_api_object.add_raw_parent(subformation_parent)

        subformation_raw_api_object.add_raw_member("ordering_priority",
                                                   4,
                                                   subformation_parent)

        pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
        pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

        # =======================================================================
        # Ranged subformation
        # =======================================================================
        subformation_ref_in_modpack = "util.formation.subformation.types.Ranged"
        subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                                   "Ranged",
                                                   api_objects,
                                                   subformation_location)
        subformation_raw_api_object.set_filename("subformations")
        subformation_raw_api_object.add_raw_parent(subformation_parent)

        subformation_raw_api_object.add_raw_member("ordering_priority",
                                                   3,
                                                   subformation_parent)

        pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
        pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

        # =======================================================================
        # Siege subformation
        # =======================================================================
        subformation_ref_in_modpack = "util.formation.subformation.types.Siege"
        subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                                   "Siege",
                                                   api_objects,
                                                   subformation_location)
        subformation_raw_api_object.set_filename("subformations")
        subformation_raw_api_object.add_raw_parent(subformation_parent)

        subformation_raw_api_object.add_raw_member("ordering_priority",
                                                   2,
                                                   subformation_parent)

        pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
        pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

        # =======================================================================
        # Support subformation
        # =======================================================================
        subformation_ref_in_modpack = "util.formation.subformation.types.Support"
        subformation_raw_api_object = RawAPIObject(subformation_ref_in_modpack,
                                                   "Support",
                                                   api_objects,
                                                   subformation_location)
        subformation_raw_api_object.set_filename("subformations")
        subformation_raw_api_object.add_raw_parent(subformation_parent)

        subformation_raw_api_object.add_raw_member("ordering_priority",
                                                   1,
                                                   subformation_parent)

        pregen_converter_group.add_raw_api_object(subformation_raw_api_object)
        pregen_nyan_objects.update({subformation_ref_in_modpack: subformation_raw_api_object})

    @staticmethod
    def generate_language_objects(full_data_set, pregen_converter_group):
        """
        Generate language objects from the string resources

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        language_parent = "engine.util.language.Language"
        language_location = "data/util/language/"

        languages = full_data_set.strings.get_tables().keys()

        for language in languages:
            language_ref_in_modpack = f"util.language.{language}"
            language_raw_api_object = RawAPIObject(language_ref_in_modpack,
                                                   language,
                                                   api_objects,
                                                   language_location)
            language_raw_api_object.set_filename("language")
            language_raw_api_object.add_raw_parent(language_parent)

            language_raw_api_object.add_raw_member("ietf_string",
                                                   language,
                                                   language_parent)

            pregen_converter_group.add_raw_api_object(language_raw_api_object)
            pregen_nyan_objects.update({language_ref_in_modpack: language_raw_api_object})

    @staticmethod
    def generate_misc_effect_objects(full_data_set, pregen_converter_group):
        """
        Generate fallback types and other standard objects for effects and resistances.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # Min change value (lower cealing for attack effects)
        # =======================================================================
        min_change_parent = "engine.util.attribute.AttributeAmount"
        min_change_location = "data/effect/discrete/flat_attribute_change/"

        change_ref_in_modpack = "effect.discrete.flat_attribute_change.min_damage.AoE2MinChangeAmount"
        change_raw_api_object = RawAPIObject(change_ref_in_modpack,
                                             "AoE2MinChangeAmount",
                                             api_objects,
                                             min_change_location)
        change_raw_api_object.set_filename("min_damage")
        change_raw_api_object.add_raw_parent(min_change_parent)

        attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
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
        min_change_parent = "engine.util.attribute.AttributeRate"
        min_change_location = "data/effect/discrete/flat_attribute_change/"

        change_ref_in_modpack = "effect.discrete.flat_attribute_change.min_heal.AoE2MinChangeAmount"
        change_raw_api_object = RawAPIObject(change_ref_in_modpack,
                                             "AoE2MinChangeAmount",
                                             api_objects,
                                             min_change_location)
        change_raw_api_object.set_filename("min_heal")
        change_raw_api_object.add_raw_parent(min_change_parent)

        attribute = ForwardRef(pregen_converter_group, "util.attribute.types.Health")
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
        type_ref = "engine.util.attribute_change_type.type.Fallback"
        change_type = api_objects[type_ref]
        fallback_raw_api_object.add_raw_member("type",
                                               change_type,
                                               effect_parent)

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
                                               effect_parent)

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
        type_ref = "engine.util.attribute_change_type.type.Fallback"
        change_type = api_objects[type_ref]
        fallback_raw_api_object.add_raw_member("type",
                                               change_type,
                                               effect_parent)

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
                                               effect_parent)

        pregen_converter_group.add_raw_api_object(fallback_raw_api_object)
        pregen_nyan_objects.update({fallback_ref_in_modpack: fallback_raw_api_object})

        # =======================================================================
        # Property Construct
        # =======================================================================
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

        # =======================================================================
        # Property Repair
        # =======================================================================
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

    @staticmethod
    def generate_modifiers(full_data_set, pregen_converter_group):
        """
        Generate standard modifiers.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        modifier_parent = "engine.modifier.Modifier"
        mprop_parent = "engine.modifier.property.type.Multiplier"
        type_parent = "engine.modifier.effect.flat_attribute_change.type.Flyover"
        types_location = "data/util/modifier/flyover_cliff/"

        # =======================================================================
        # Flyover effect multiplier
        # =======================================================================
        modifier_ref_in_modpack = "util.modifier.flyover_cliff.AttackFlyover"
        modifier_raw_api_object = RawAPIObject(modifier_ref_in_modpack,
                                               "AttackFlyover", api_objects,
                                               types_location)
        modifier_raw_api_object.set_filename("flyover_cliff")
        modifier_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(modifier_raw_api_object)
        pregen_nyan_objects.update({modifier_ref_in_modpack: modifier_raw_api_object})

        # Relative angle to cliff must not be smaller than 90°
        modifier_raw_api_object.add_raw_member("relative_angle",
                                               90,
                                               type_parent)

        # Affects all cliffs
        types = [ForwardRef(pregen_converter_group, "util.game_entity_type.types.Cliff")]
        modifier_raw_api_object.add_raw_member("flyover_types",
                                               types,
                                               type_parent)
        modifier_raw_api_object.add_raw_member("blacklisted_entities",
                                               [],
                                               type_parent)

        # Multiplier property: Increases effect value by 25%
        # --------------------------------------------------
        prop_ref_in_modpack = "util.modifier.flyover_cliff.AttackFlyover.Multiplier"
        prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                           "Multiplier", api_objects,
                                           types_location)
        prop_location = ForwardRef(pregen_converter_group, modifier_ref_in_modpack)
        prop_raw_api_object.set_location(prop_location)
        prop_raw_api_object.add_raw_parent(mprop_parent)

        pregen_converter_group.add_raw_api_object(prop_raw_api_object)
        pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

        prop_raw_api_object.add_raw_member("multiplier",
                                           1.25,
                                           mprop_parent)
        # --------------------------------------------------
        # Assign property to modifier
        prop_forward_ref = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
        properties = {api_objects[mprop_parent]: prop_forward_ref}
        modifier_raw_api_object.add_raw_member("properties",
                                               properties,
                                               modifier_parent)

        # =======================================================================
        # Elevation difference effect multiplier (higher unit)
        # =======================================================================
        type_parent = "engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceHigh"
        types_location = "data/util/modifier/elevation_difference/"

        modifier_ref_in_modpack = "util.modifier.elevation_difference.AttackHigh"
        modifier_raw_api_object = RawAPIObject(modifier_ref_in_modpack,
                                               "AttackHigh", api_objects,
                                               types_location)
        modifier_raw_api_object.set_filename("elevation_difference")
        modifier_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(modifier_raw_api_object)
        pregen_nyan_objects.update({modifier_ref_in_modpack: modifier_raw_api_object})

        # Multiplier property: Increases effect value to 125%
        # --------------------------------------------------
        prop_ref_in_modpack = "util.modifier.elevation_difference.AttackHigh.Multiplier"
        prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                           "Multiplier", api_objects,
                                           types_location)
        prop_location = ForwardRef(pregen_converter_group, modifier_ref_in_modpack)
        prop_raw_api_object.set_location(prop_location)
        prop_raw_api_object.add_raw_parent(mprop_parent)

        pregen_converter_group.add_raw_api_object(prop_raw_api_object)
        pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

        prop_raw_api_object.add_raw_member("multiplier",
                                           1.25,
                                           mprop_parent)
        # --------------------------------------------------
        # Assign property to modifier
        prop_forward_ref = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
        properties = {api_objects[mprop_parent]: prop_forward_ref}
        modifier_raw_api_object.add_raw_member("properties",
                                               properties,
                                               modifier_parent)

        # =======================================================================
        # Elevation difference effect multiplier (lower unit)
        # =======================================================================
        type_parent = "engine.modifier.effect.flat_attribute_change.type.ElevationDifferenceLow"
        types_location = "data/util/modifier/elevation_difference/"

        modifier_ref_in_modpack = "util.modifier.elevation_difference.AttackLow"
        modifier_raw_api_object = RawAPIObject(modifier_ref_in_modpack,
                                               "AttackLow", api_objects,
                                               types_location)
        modifier_raw_api_object.set_filename("elevation_difference")
        modifier_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(modifier_raw_api_object)
        pregen_nyan_objects.update({modifier_ref_in_modpack: modifier_raw_api_object})

        # Multiplier property: Decreases effect value to 75%
        # --------------------------------------------------
        prop_ref_in_modpack = "util.modifier.elevation_difference.AttackLow.Multiplier"
        prop_raw_api_object = RawAPIObject(prop_ref_in_modpack,
                                           "Multiplier", api_objects,
                                           types_location)
        prop_location = ForwardRef(pregen_converter_group, modifier_ref_in_modpack)
        prop_raw_api_object.set_location(prop_location)
        prop_raw_api_object.add_raw_parent(mprop_parent)

        pregen_converter_group.add_raw_api_object(prop_raw_api_object)
        pregen_nyan_objects.update({prop_ref_in_modpack: prop_raw_api_object})

        prop_raw_api_object.add_raw_member("multiplier",
                                           1.25,
                                           mprop_parent)
        # --------------------------------------------------
        # Assign property to modifier
        prop_forward_ref = ForwardRef(pregen_converter_group, prop_ref_in_modpack)
        properties = {api_objects[mprop_parent]: prop_forward_ref}
        modifier_raw_api_object.add_raw_member("properties",
                                               properties,
                                               modifier_parent)

    @staticmethod
    def generate_terrain_types(full_data_set, pregen_converter_group):
        """
        Generate TerrainType objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        terrain_type_lookup_dict = internal_name_lookups.get_terrain_type_lookups(full_data_set.game_version)

        type_parent = "engine.util.terrain_type.TerrainType"
        types_location = "data/util/terrain_type/"

        terrain_type_lookups = terrain_type_lookup_dict.values()

        for terrain_type in terrain_type_lookups:
            type_name = terrain_type[2]
            type_ref_in_modpack = f"util.terrain_type.types.{type_name}"
            type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                               type_name, api_objects,
                                               types_location)
            type_raw_api_object.set_filename("types")
            type_raw_api_object.add_raw_parent(type_parent)

            pregen_converter_group.add_raw_api_object(type_raw_api_object)
            pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    @staticmethod
    def generate_resources(full_data_set, pregen_converter_group):
        """
        Generate Attribute objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        resource_parent = "engine.util.resource.Resource"
        resources_location = "data/util/resource/"

        # =======================================================================
        # Food
        # =======================================================================
        food_ref_in_modpack = "util.resource.types.Food"
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

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        food_name_ref_in_modpack = "util.attribute.types.Food.FoodName"
        food_name_value = RawAPIObject(food_name_ref_in_modpack, "FoodName",
                                       api_objects, resources_location)
        food_name_value.set_filename("types")
        food_name_value.add_raw_parent(name_value_parent)
        food_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      food_name_ref_in_modpack)
        food_raw_api_object.add_raw_member("name",
                                           name_forward_ref,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(food_name_value)
        pregen_nyan_objects.update({food_name_ref_in_modpack: food_name_value})

        # =======================================================================
        # Wood
        # =======================================================================
        wood_ref_in_modpack = "util.resource.types.Wood"
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

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        wood_name_ref_in_modpack = "util.attribute.types.Wood.WoodName"
        wood_name_value = RawAPIObject(wood_name_ref_in_modpack, "WoodName",
                                       api_objects, resources_location)
        wood_name_value.set_filename("types")
        wood_name_value.add_raw_parent(name_value_parent)
        wood_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      wood_name_ref_in_modpack)
        wood_raw_api_object.add_raw_member("name",
                                           name_forward_ref,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(wood_name_value)
        pregen_nyan_objects.update({wood_name_ref_in_modpack: wood_name_value})

        # =======================================================================
        # Stone
        # =======================================================================
        stone_ref_in_modpack = "util.resource.types.Stone"
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

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        stone_name_ref_in_modpack = "util.attribute.types.Stone.StoneName"
        stone_name_value = RawAPIObject(stone_name_ref_in_modpack, "StoneName",
                                        api_objects, resources_location)
        stone_name_value.set_filename("types")
        stone_name_value.add_raw_parent(name_value_parent)
        stone_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      stone_name_ref_in_modpack)
        stone_raw_api_object.add_raw_member("name",
                                            name_forward_ref,
                                            resource_parent)

        pregen_converter_group.add_raw_api_object(stone_name_value)
        pregen_nyan_objects.update({stone_name_ref_in_modpack: stone_name_value})

        # =======================================================================
        # Gold
        # =======================================================================
        gold_ref_in_modpack = "util.resource.types.Gold"
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

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        gold_name_ref_in_modpack = "util.attribute.types.Gold.GoldName"
        gold_name_value = RawAPIObject(gold_name_ref_in_modpack, "GoldName",
                                       api_objects, resources_location)
        gold_name_value.set_filename("types")
        gold_name_value.add_raw_parent(name_value_parent)
        gold_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      gold_name_ref_in_modpack)
        gold_raw_api_object.add_raw_member("name",
                                           name_forward_ref,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(gold_name_value)
        pregen_nyan_objects.update({gold_name_ref_in_modpack: gold_name_value})

        # =======================================================================
        # Population Space
        # =======================================================================
        resource_contingent_parent = "engine.util.resource.ResourceContingent"

        pop_ref_in_modpack = "util.resource.types.PopulationSpace"
        pop_raw_api_object = RawAPIObject(pop_ref_in_modpack,
                                          "PopulationSpace", api_objects,
                                          resources_location)
        pop_raw_api_object.set_filename("types")
        pop_raw_api_object.add_raw_parent(resource_contingent_parent)

        pregen_converter_group.add_raw_api_object(pop_raw_api_object)
        pregen_nyan_objects.update({pop_ref_in_modpack: pop_raw_api_object})

        name_value_parent = "engine.util.language.translated.type.TranslatedString"
        pop_name_ref_in_modpack = "util.attribute.types.PopulationSpace.PopulationSpaceName"
        pop_name_value = RawAPIObject(pop_name_ref_in_modpack, "PopulationSpaceName",
                                      api_objects, resources_location)
        pop_name_value.set_filename("types")
        pop_name_value.add_raw_parent(name_value_parent)
        pop_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      pop_name_ref_in_modpack)
        pop_raw_api_object.add_raw_member("name",
                                          name_forward_ref,
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
    def generate_death_condition(full_data_set, pregen_converter_group):
        """
        Generate DeathCondition objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        # =======================================================================
        # Death condition
        # =======================================================================
        logic_parent = "engine.util.logic.LogicElement"
        literal_parent = "engine.util.logic.literal.Literal"
        interval_parent = "engine.util.logic.literal.type.AttributeBelowValue"
        literal_location = "data/util/logic/death/"

        death_ref_in_modpack = "util.logic.literal.death.StandardHealthDeathLiteral"
        literal_raw_api_object = RawAPIObject(death_ref_in_modpack,
                                              "StandardHealthDeathLiteral",
                                              api_objects,
                                              literal_location)
        literal_raw_api_object.set_filename("death")
        literal_raw_api_object.add_raw_parent(interval_parent)

        # Literal will not default to 'True' when it was fulfilled once
        literal_raw_api_object.add_raw_member("only_once", False, logic_parent)

        # Scope
        scope_forward_ref = ForwardRef(pregen_converter_group,
                                       "util.logic.literal_scope.death.StandardHealthDeathScope")
        literal_raw_api_object.add_raw_member("scope",
                                              scope_forward_ref,
                                              literal_parent)

        # Attribute
        health_forward_ref = ForwardRef(pregen_converter_group,
                                        "util.attribute.types.Health")
        literal_raw_api_object.add_raw_member("attribute",
                                              health_forward_ref,
                                              interval_parent)

        # sidenote: Apparently this is actually HP<1 in Genie
        # (https://youtu.be/FdBk8zGbE7U?t=7m16s)
        literal_raw_api_object.add_raw_member("threshold",
                                              1,
                                              interval_parent)

        pregen_converter_group.add_raw_api_object(literal_raw_api_object)
        pregen_nyan_objects.update({death_ref_in_modpack: literal_raw_api_object})

        # LiteralScope
        scope_parent = "engine.util.logic.literal_scope.LiteralScope"
        self_scope_parent = "engine.util.logic.literal_scope.type.Self"

        death_scope_ref_in_modpack = "util.logic.literal_scope.death.StandardHealthDeathScope"
        scope_raw_api_object = RawAPIObject(death_scope_ref_in_modpack,
                                            "StandardHealthDeathScope",
                                            api_objects)
        scope_location = ForwardRef(pregen_converter_group, death_ref_in_modpack)
        scope_raw_api_object.set_location(scope_location)
        scope_raw_api_object.add_raw_parent(self_scope_parent)

        scope_diplomatic_stances = [api_objects["engine.util.diplomatic_stance.type.Self"]]
        scope_raw_api_object.add_raw_member("stances",
                                            scope_diplomatic_stances,
                                            scope_parent)

        pregen_converter_group.add_raw_api_object(scope_raw_api_object)
        pregen_nyan_objects.update({death_scope_ref_in_modpack: scope_raw_api_object})

        # =======================================================================
        # Garrison empty condition
        # =======================================================================
        logic_parent = "engine.util.logic.LogicElement"
        literal_parent = "engine.util.logic.literal.Literal"
        interval_parent = "engine.util.logic.literal.type.AttributeBelowValue"
        literal_location = "data/util/logic/garrison_empty/"

        garrison_literal_ref_in_modpack = "util.logic.literal.garrison.BuildingDamageEmpty"
        literal_raw_api_object = RawAPIObject(garrison_literal_ref_in_modpack,
                                              "BuildingDamageEmptyLiteral",
                                              api_objects,
                                              literal_location)
        literal_raw_api_object.set_filename("garrison_empty")
        literal_raw_api_object.add_raw_parent(interval_parent)

        # Literal will not default to 'True' when it was fulfilled once
        literal_raw_api_object.add_raw_member("only_once", False, logic_parent)

        # Scope
        scope_forward_ref = ForwardRef(pregen_converter_group,
                                       "util.logic.literal_scope.garrison.BuildingDamageEmptyScope")
        literal_raw_api_object.add_raw_member("scope",
                                              scope_forward_ref,
                                              literal_parent)

        # Attribute
        health_forward_ref = ForwardRef(pregen_converter_group,
                                        "util.attribute.types.Health")
        literal_raw_api_object.add_raw_member("attribute",
                                              health_forward_ref,
                                              interval_parent)

        # Threshhold
        literal_raw_api_object.add_raw_member("threshold",
                                              0.2,
                                              interval_parent)

        pregen_converter_group.add_raw_api_object(literal_raw_api_object)
        pregen_nyan_objects.update({garrison_literal_ref_in_modpack: literal_raw_api_object})

        # LiteralScope
        scope_parent = "engine.util.logic.literal_scope.LiteralScope"
        self_scope_parent = "engine.util.logic.literal_scope.type.Self"

        garrison_scope_ref_in_modpack = "util.logic.literal_scope.garrison.BuildingDamageEmptyScope"
        scope_raw_api_object = RawAPIObject(garrison_scope_ref_in_modpack,
                                            "BuildingDamageEmptyScope",
                                            api_objects)
        scope_location = ForwardRef(pregen_converter_group, garrison_literal_ref_in_modpack)
        scope_raw_api_object.set_location(scope_location)
        scope_raw_api_object.add_raw_parent(self_scope_parent)

        scope_diplomatic_stances = [api_objects["engine.util.diplomatic_stance.type.Self"]]
        scope_raw_api_object.add_raw_member("stances",
                                            scope_diplomatic_stances,
                                            scope_parent)

        pregen_converter_group.add_raw_api_object(scope_raw_api_object)
        pregen_nyan_objects.update({garrison_scope_ref_in_modpack: scope_raw_api_object})

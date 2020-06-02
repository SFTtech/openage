# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.
"""
from openage.convert.dataformat.aoc.forward_ref import ForwardRef
from openage.convert.dataformat.converter_object import ConverterObjectGroup,\
    RawAPIObject
from openage.convert.dataformat.swgbcc.swgb_unit import SWGBUnitTransformGroup
from openage.convert.processor.aoc.pregen_processor import AoCPregenSubprocessor
from openage.convert.service import internal_name_lookups
from openage.nyan.nyan_structs import MemberSpecialValue


class SWGBPregenSubprocessor:

    @classmethod
    def generate(cls, gamedata):
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        AoCPregenSubprocessor._generate_attributes(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_diplomatic_stances(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_entity_types(gamedata, pregen_converter_group)
        cls._generate_effect_types(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_exchange_objects(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_formation_types(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_language_objects(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_misc_effect_objects(gamedata, pregen_converter_group)
        # cls._generate_modifiers(gamedata, pregen_converter_group) ??
        # cls._generate_terrain_types(gamedata, pregen_converter_group) TODO: Create terrain types
        cls._generate_resources(gamedata, pregen_converter_group)
        AoCPregenSubprocessor._generate_death_condition(gamedata, pregen_converter_group)

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
    def _generate_effect_types(full_data_set, pregen_converter_group):
        """
        Generate types for effects and resistances.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
        :type pregen_converter_group: class: ...dataformat.aoc.genie_object_container.GenieObjectGroup
        """
        pregen_nyan_objects = full_data_set.pregen_nyan_objects
        api_objects = full_data_set.nyan_api_objects

        name_lookup_dict = internal_name_lookups.get_entity_lookups(full_data_set.game_version)
        armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(full_data_set.game_version)

        # =======================================================================
        # Armor types
        # =======================================================================
        type_parent = "engine.aux.attribute_change_type.AttributeChangeType"
        types_location = "data/aux/attribute_change_type/"

        for type_name in armor_lookup_dict.values():
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
            if unit_line.is_repairable():
                repairable_lines.append(unit_line)

        for repairable_line in repairable_lines:
            if isinstance(repairable_line, SWGBUnitTransformGroup):
                game_entity_name = name_lookup_dict[repairable_line.get_transform_unit_id()][0]

            else:
                game_entity_name = name_lookup_dict[repairable_line.get_head_unit_id()][0]

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
            game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

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
            game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

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
    def _generate_resources(full_data_set, pregen_converter_group):
        """
        Generate Attribute objects.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        :param pregen_converter_group: GenieObjectGroup instance that stores
                                       pregenerated API objects for referencing with
                                       ForwardRef
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

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      food_name_ref_in_modpack)
        food_raw_api_object.add_raw_member("name",
                                           name_forward_ref,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(food_name_value)
        pregen_nyan_objects.update({food_name_ref_in_modpack: food_name_value})

        # =======================================================================
        # Carbon
        # =======================================================================
        carbon_ref_in_modpack = "aux.resource.types.Carbon"
        carbon_raw_api_object = RawAPIObject(carbon_ref_in_modpack,
                                             "Carbon", api_objects,
                                             resources_location)
        carbon_raw_api_object.set_filename("types")
        carbon_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(carbon_raw_api_object)
        pregen_nyan_objects.update({carbon_ref_in_modpack: carbon_raw_api_object})

        carbon_raw_api_object.add_raw_member("max_storage",
                                             MemberSpecialValue.NYAN_INF,
                                             resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        carbon_name_ref_in_modpack = "aux.attribute.types.Carbon.CarbonName"
        carbon_name_value = RawAPIObject(carbon_name_ref_in_modpack, "CarbonName",
                                         api_objects, resources_location)
        carbon_name_value.set_filename("types")
        carbon_name_value.add_raw_parent(name_value_parent)
        carbon_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      carbon_name_ref_in_modpack)
        carbon_raw_api_object.add_raw_member("name",
                                             name_forward_ref,
                                             resource_parent)

        pregen_converter_group.add_raw_api_object(carbon_name_value)
        pregen_nyan_objects.update({carbon_name_ref_in_modpack: carbon_name_value})

        # =======================================================================
        # Ore
        # =======================================================================
        ore_ref_in_modpack = "aux.resource.types.Ore"
        ore_raw_api_object = RawAPIObject(ore_ref_in_modpack,
                                          "Ore", api_objects,
                                          resources_location)
        ore_raw_api_object.set_filename("types")
        ore_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(ore_raw_api_object)
        pregen_nyan_objects.update({ore_ref_in_modpack: ore_raw_api_object})

        ore_raw_api_object.add_raw_member("max_storage",
                                          MemberSpecialValue.NYAN_INF,
                                          resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        ore_name_ref_in_modpack = "aux.attribute.types.Ore.OreName"
        ore_name_value = RawAPIObject(ore_name_ref_in_modpack, "OreName",
                                      api_objects, resources_location)
        ore_name_value.set_filename("types")
        ore_name_value.add_raw_parent(name_value_parent)
        ore_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      ore_name_ref_in_modpack)
        ore_raw_api_object.add_raw_member("name",
                                          name_forward_ref,
                                          resource_parent)

        pregen_converter_group.add_raw_api_object(ore_name_value)
        pregen_nyan_objects.update({ore_name_ref_in_modpack: ore_name_value})

        # =======================================================================
        # Nova
        # =======================================================================
        nova_ref_in_modpack = "aux.resource.types.Nova"
        nova_raw_api_object = RawAPIObject(nova_ref_in_modpack,
                                           "Nova", api_objects,
                                           resources_location)
        nova_raw_api_object.set_filename("types")
        nova_raw_api_object.add_raw_parent(resource_parent)

        pregen_converter_group.add_raw_api_object(nova_raw_api_object)
        pregen_nyan_objects.update({nova_ref_in_modpack: nova_raw_api_object})

        nova_raw_api_object.add_raw_member("max_storage",
                                           MemberSpecialValue.NYAN_INF,
                                           resource_parent)

        name_value_parent = "engine.aux.translated.type.TranslatedString"
        nova_name_ref_in_modpack = "aux.attribute.types.Nova.NovaName"
        nova_name_value = RawAPIObject(nova_name_ref_in_modpack, "NovaName",
                                       api_objects, resources_location)
        nova_name_value.set_filename("types")
        nova_name_value.add_raw_parent(name_value_parent)
        nova_name_value.add_raw_member("translations", [], name_value_parent)

        name_forward_ref = ForwardRef(pregen_converter_group,
                                      nova_name_ref_in_modpack)
        nova_raw_api_object.add_raw_member("name",
                                           name_forward_ref,
                                           resource_parent)

        pregen_converter_group.add_raw_api_object(nova_name_value)
        pregen_nyan_objects.update({nova_name_ref_in_modpack: nova_name_value})

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

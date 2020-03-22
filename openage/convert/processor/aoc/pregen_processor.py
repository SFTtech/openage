# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.

"""
from openage.convert.dataformat.converter_object import RawAPIObject,\
    ConverterObjectGroup
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.nyan.nyan_structs import MemberSpecialValue


class AoCPregenSubprocessor:

    @classmethod
    def generate(cls, gamedata):
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        cls._generate_attributes(gamedata, pregen_converter_group)
        cls._generate_entity_types(gamedata, pregen_converter_group)
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
    def _generate_entity_types(full_data_set, pregen_converter_group):
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
                                          MemberSpecialValue.NYAN_INF,
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
                                              False,
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
                                              0,
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

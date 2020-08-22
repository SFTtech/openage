# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals

"""
Creates nyan objects for things that are hardcoded into the Genie Engine,
but configurable in openage. E.g. HP.
"""
from ....entity_object.conversion.converter_object import ConverterObjectGroup,\
    RawAPIObject
from ....value_object.conversion.forward_ref import ForwardRef
from ..aoc.pregen_processor import AoCPregenSubprocessor


class RoRPregenSubprocessor:
    """
    Creates raw API objects for hardcoded settings in RoR.
    """

    @classmethod
    def generate(cls, gamedata):
        """
        Create nyan objects for hardcoded properties.
        """
        # Stores pregenerated raw API objects as a container
        pregen_converter_group = ConverterObjectGroup("pregen")

        AoCPregenSubprocessor.generate_attributes(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_diplomatic_stances(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_entity_types(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_effect_types(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_language_objects(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_misc_effect_objects(gamedata, pregen_converter_group)
        # TODO:
        # cls._generate_modifiers(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_terrain_types(gamedata, pregen_converter_group)
        AoCPregenSubprocessor.generate_resources(gamedata, pregen_converter_group)
        cls.generate_death_condition(gamedata, pregen_converter_group)

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
        logic_parent = "engine.aux.logic.LogicElement"
        literal_parent = "engine.aux.logic.literal.Literal"
        interval_parent = "engine.aux.logic.literal.type.AttributeBelowValue"
        literal_location = "data/aux/logic/death/"

        death_ref_in_modpack = "aux.logic.literal.death.StandardHealthDeathLiteral"
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
                                       "aux.logic.literal_scope.death.StandardHealthDeathScope")
        literal_raw_api_object.add_raw_member("scope",
                                              scope_forward_ref,
                                              literal_parent)

        # Attribute
        health_forward_ref = ForwardRef(pregen_converter_group,
                                        "aux.attribute.types.Health")
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
        scope_parent = "engine.aux.logic.literal_scope.LiteralScope"
        self_scope_parent = "engine.aux.logic.literal_scope.type.Self"

        death_scope_ref_in_modpack = "aux.logic.literal_scope.death.StandardHealthDeathScope"
        scope_raw_api_object = RawAPIObject(death_scope_ref_in_modpack,
                                            "StandardHealthDeathScope",
                                            api_objects)
        scope_location = ForwardRef(pregen_converter_group, death_ref_in_modpack)
        scope_raw_api_object.set_location(scope_location)
        scope_raw_api_object.add_raw_parent(self_scope_parent)

        scope_diplomatic_stances = [api_objects["engine.aux.diplomatic_stance.type.Self"]]
        scope_raw_api_object.add_raw_member("diplomatic_stances",
                                            scope_diplomatic_stances,
                                            scope_parent)

        pregen_converter_group.add_raw_api_object(scope_raw_api_object)
        pregen_nyan_objects.update({death_scope_ref_in_modpack: scope_raw_api_object})

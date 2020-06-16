# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Convert data from DE2 to openage formats.
"""

from openage.convert.dataformat.aoc.genie_graphic import GenieGraphic
from openage.convert.dataformat.aoc.genie_object_container import GenieObjectContainer
from openage.convert.dataformat.aoc.genie_unit import GenieUnitObject,\
    GenieAmbientGroup, GenieVariantGroup
import openage.convert.dataformat.aoc.internal_nyan_names as aoc_internal
import openage.convert.dataformat.de2.internal_nyan_names as de2_internal
from openage.convert.nyan.api_loader import load_api
from openage.convert.processor.aoc.pregen_processor import AoCPregenSubprocessor
from openage.convert.processor.aoc.processor import AoCProcessor
from openage.convert.processor.de2.media_subprocessor import DE2MediaSubprocessor
from openage.convert.processor.de2.modpack_subprocessor import DE2ModpackSubprocessor
from openage.convert.processor.de2.nyan_subprocessor import DE2NyanSubprocessor
from openage.util.ordered_set import OrderedSet

from ....log import info


class DE2Processor:

    @classmethod
    def convert(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Input game speification and media here and get a set of
        modpacks back.

        :param gamespec: Gamedata from empires.dat read in by the
                         reader functions.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :returns: A list of modpacks.
        :rtype: list
        """

        info("Starting conversion...")

        # Create a new container for the conversion process
        data_set = cls._pre_processor(gamespec, game_version, string_resources, existing_graphics)

        # Create the custom openae formats (nyan, sprite, terrain)
        data_set = cls._processor(data_set)

        # Create modpack definitions
        modpacks = cls._post_processor(data_set)

        return modpacks

    @classmethod
    def _pre_processor(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Store data from the reader in a conversion container.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        dataset = GenieObjectContainer()

        dataset.game_version = game_version
        dataset.nyan_api_objects = load_api()
        dataset.strings = string_resources
        dataset.existing_graphics = existing_graphics

        info("Extracting Genie data...")

        cls._extract_genie_units(gamespec, dataset)
        AoCProcessor._extract_genie_techs(gamespec, dataset)
        AoCProcessor._extract_genie_effect_bundles(gamespec, dataset)
        AoCProcessor._sanitize_effect_bundles(dataset)
        AoCProcessor._extract_genie_civs(gamespec, dataset)
        AoCProcessor._extract_age_connections(gamespec, dataset)
        AoCProcessor._extract_building_connections(gamespec, dataset)
        AoCProcessor._extract_unit_connections(gamespec, dataset)
        AoCProcessor._extract_tech_connections(gamespec, dataset)
        cls._extract_genie_graphics(gamespec, dataset)
        AoCProcessor._extract_genie_sounds(gamespec, dataset)
        AoCProcessor._extract_genie_terrains(gamespec, dataset)

        return dataset

    @classmethod
    def _processor(cls, full_data_set):
        """
        1. Transfer structures used in Genie games to more openage-friendly
           Python objects.
        2. Convert these objects to nyan.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        AoCProcessor._create_unit_lines(full_data_set)
        AoCProcessor._create_extra_unit_lines(full_data_set)
        AoCProcessor._create_building_lines(full_data_set)
        AoCProcessor._create_villager_groups(full_data_set)
        cls._create_ambient_groups(full_data_set)
        cls._create_variant_groups(full_data_set)
        AoCProcessor._create_terrain_groups(full_data_set)
        AoCProcessor._create_tech_groups(full_data_set)
        AoCProcessor._create_node_tech_groups(full_data_set)
        AoCProcessor._create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        AoCProcessor._link_building_upgrades(full_data_set)
        AoCProcessor._link_creatables(full_data_set)
        AoCProcessor._link_researchables(full_data_set)
        AoCProcessor._link_civ_uniques(full_data_set)
        AoCProcessor._link_gatherers_to_dropsites(full_data_set)
        AoCProcessor._link_garrison(full_data_set)
        AoCProcessor._link_trade_posts(full_data_set)
        AoCProcessor._link_repairables(full_data_set)

        info("Generating auxiliary objects...")

        AoCPregenSubprocessor.generate(full_data_set)

        return full_data_set

    @classmethod
    def _post_processor(cls, full_data_set):

        info("Creating nyan objects...")

        DE2NyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        DE2MediaSubprocessor.convert(full_data_set)

        return DE2ModpackSubprocessor.get_modpacks(full_data_set)

    @staticmethod
    def _extract_genie_units(gamespec, full_data_set):
        """
        Extract units from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # Units are stored in the civ container.
        # All civs point to the same units (?) except for Gaia which has more.
        # Gaia also seems to have the most units, so we only read from Gaia
        #
        # call hierarchy: wrapper[0]->civs[0]->units
        raw_units = gamespec.get_value()[0].get_value()["civs"].get_value()[0]\
            .get_value()["units"].get_value()

        # Unit headers store the things units can do
        raw_unit_headers = gamespec.get_value()[0].get_value()["unit_headers"].get_value()

        for raw_unit in raw_units:
            unit_id = raw_unit.get_value()["id0"].get_value()
            unit_members = raw_unit.get_value()

            # Turn attack and armor into containers to make diffing work
            if "attacks" in unit_members.keys():
                attacks_member = unit_members.pop("attacks")
                attacks_member = attacks_member.get_container("type_id")
                armors_member = unit_members.pop("armors")
                armors_member = armors_member.get_container("type_id")

                unit_members.update({"attacks": attacks_member})
                unit_members.update({"armors": armors_member})

            unit = GenieUnitObject(unit_id, full_data_set, members=unit_members)
            full_data_set.genie_units.update({unit.get_id(): unit})

            # Commands
            if "unit_commands" not in unit_members.keys():
                unit_commands = raw_unit_headers[unit_id].get_value()["unit_commands"]
                unit.add_member(unit_commands)

    @staticmethod
    def _extract_genie_graphics(gamespec, full_data_set):
        """
        Extract graphic definitions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->graphics
        raw_graphics = gamespec.get_value()[0].get_value()["graphics"].get_value()

        for raw_graphic in raw_graphics:
            # Can be ignored if there is no filename associated
            filename = raw_graphic.get_value()["filename"].get_value()
            if not filename:
                continue

            graphic_id = raw_graphic.get_value()["graphic_id"].get_value()
            graphic_members = raw_graphic.get_value()
            graphic = GenieGraphic(graphic_id, full_data_set, members=graphic_members)

            if filename not in full_data_set.existing_graphics:
                graphic.exists = False

            full_data_set.genie_graphics.update({graphic.get_id(): graphic})

        # Detect subgraphics
        for genie_graphic in full_data_set.genie_graphics.values():
            genie_graphic.detect_subgraphics()

    @staticmethod
    def _create_ambient_groups(full_data_set):
        """
        Create ambient groups, mostly for resources and scenery.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        ambient_ids = OrderedSet()
        ambient_ids.update(aoc_internal.AMBIENT_GROUP_LOOKUPS.keys())
        ambient_ids.update(de2_internal.AMBIENT_GROUP_LOOKUPS.keys())
        genie_units = full_data_set.genie_units

        for ambient_id in ambient_ids:
            ambient_group = GenieAmbientGroup(ambient_id, full_data_set)
            ambient_group.add_unit(genie_units[ambient_id])
            full_data_set.ambient_groups.update({ambient_group.get_id(): ambient_group})
            full_data_set.unit_ref.update({ambient_id: ambient_group})

    @staticmethod
    def _create_variant_groups(full_data_set):
        """
        Create variant groups.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        variants = {}
        variants.update(aoc_internal.VARIANT_GROUP_LOOKUPS)
        variants.update(de2_internal.VARIANT_GROUP_LOOKUPS)

        for group_id, variant in variants.items():
            variant_group = GenieVariantGroup(group_id, full_data_set)
            full_data_set.variant_groups.update({variant_group.get_id(): variant_group})

            for variant_id in variant[2]:
                variant_group.add_unit(full_data_set.genie_units[variant_id])
                full_data_set.unit_ref.update({variant_id: variant_group})

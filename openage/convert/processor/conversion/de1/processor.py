# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Convert data from DE1 to openage formats.
"""


from .....log import info
from ....entity_object.conversion.aoc.genie_graphic import GenieGraphic
from ....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer
from ....service.debug_info import debug_converter_objects,\
    debug_converter_object_groups
from ....service.read.nyan_api_loader import load_api
from ..aoc.processor import AoCProcessor
from ..ror.modpack_subprocessor import RoRModpackSubprocessor
from ..ror.nyan_subprocessor import RoRNyanSubprocessor
from ..ror.pregen_subprocessor import RoRPregenSubprocessor
from ..ror.processor import RoRProcessor
from .media_subprocessor import DE1MediaSubprocessor


class DE1Processor:
    """
    Main processor for converting data from DE1.
    """

    @classmethod
    def convert(cls, gamespec, args, string_resources, existing_graphics):
        """
        Input game specification and media here and get a set of
        modpacks back.

        :param gamespec: Gamedata from empires.dat read in by the
                         reader functions.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :returns: A list of modpacks.
        :rtype: list
        """

        info("Starting conversion...")

        # Create a new container for the conversion process
        dataset = cls._pre_processor(
            gamespec,
            args.game_version,
            string_resources,
            existing_graphics
        )
        debug_converter_objects(args.debugdir, args.debug_info, dataset)

        # Create the custom openage formats (nyan, sprite, terrain)
        dataset = cls._processor(gamespec, dataset)
        debug_converter_object_groups(args.debugdir, args.debug_info, dataset)

        # Create modpack definitions
        modpacks = cls._post_processor(dataset)

        return modpacks

    @classmethod
    def _pre_processor(cls, gamespec, game_version, string_resources, existing_graphics):
        """
        Store data from the reader in a conversion container.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        dataset = GenieObjectContainer()

        dataset.game_version = game_version
        dataset.nyan_api_objects = load_api()
        dataset.strings = string_resources
        dataset.existing_graphics = existing_graphics

        info("Extracting Genie data...")

        RoRProcessor.extract_genie_units(gamespec, dataset)
        AoCProcessor.extract_genie_techs(gamespec, dataset)
        AoCProcessor.extract_genie_effect_bundles(gamespec, dataset)
        AoCProcessor.sanitize_effect_bundles(dataset)
        AoCProcessor.extract_genie_civs(gamespec, dataset)
        cls.extract_genie_graphics(gamespec, dataset)
        RoRProcessor.extract_genie_sounds(gamespec, dataset)
        AoCProcessor.extract_genie_terrains(gamespec, dataset)

        return dataset

    @classmethod
    def _processor(cls, gamespec, full_data_set):
        """
        Transfer structures used in Genie games to more openage-friendly
        Python objects.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: class: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """

        info("Creating API-like objects...")

        RoRProcessor.create_tech_groups(full_data_set)
        RoRProcessor.create_entity_lines(gamespec, full_data_set)
        RoRProcessor.create_ambient_groups(full_data_set)
        RoRProcessor.create_variant_groups(full_data_set)
        AoCProcessor.create_terrain_groups(full_data_set)
        AoCProcessor.create_civ_groups(full_data_set)

        info("Linking API-like objects...")

        AoCProcessor.link_creatables(full_data_set)
        AoCProcessor.link_researchables(full_data_set)
        AoCProcessor.link_gatherers_to_dropsites(full_data_set)
        RoRProcessor.link_garrison(full_data_set)
        AoCProcessor.link_trade_posts(full_data_set)
        RoRProcessor.link_repairables(full_data_set)

        info("Generating auxiliary objects...")

        RoRPregenSubprocessor.generate(full_data_set)

        return full_data_set

    @classmethod
    def _post_processor(cls, full_data_set):
        """
        Convert API-like Python objects to nyan.

        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :type full_data_set: ...dataformat.aoc.genie_object_container.GenieObjectContainer
        """
        info("Creating nyan objects...")

        RoRNyanSubprocessor.convert(full_data_set)

        info("Creating requests for media export...")

        DE1MediaSubprocessor.convert(full_data_set)

        return RoRModpackSubprocessor.get_modpacks(full_data_set)

    @staticmethod
    def extract_genie_graphics(gamespec, full_data_set):
        """
        Extract graphic definitions from the game data.

        :param gamespec: Gamedata from empires.dat file.
        :type gamespec: class: ...dataformat.value_members.ArrayMember
        """
        # call hierarchy: wrapper[0]->graphics
        raw_graphics = gamespec[0]["graphics"].get_value()

        for raw_graphic in raw_graphics:
            # Can be ignored if there is no filename associated
            filename = raw_graphic["filename"].get_value()
            if not filename:
                continue

            # DE1 stores most graphics filenames as 'whatever_<x#>'
            # where '<x#>' must be replaced by x1, x2 or x4
            # which corresponds to the graphics resolution variant
            #
            # we look for the x1 variant
            if filename.endswith("<x#>"):
                filename = f"{filename[:-4]}x1"

            graphic_id = raw_graphic["graphic_id"].get_value()
            graphic_members = raw_graphic.get_value()

            graphic = GenieGraphic(graphic_id, full_data_set, members=graphic_members)
            if filename not in full_data_set.existing_graphics:
                graphic.exists = False

            full_data_set.genie_graphics.update({graphic.get_id(): graphic})

        # Detect subgraphics
        for genie_graphic in full_data_set.genie_graphics.values():
            genie_graphic.detect_subgraphics()
